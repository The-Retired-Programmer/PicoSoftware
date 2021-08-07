/**
 * Copyright 2021 Richard Linsdale (richard at theretiredprogrammer.uk).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//
// PIO and DMA management
//

#include <stdlib.h>
#include <string.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/structs/bus_ctrl.h"
#include "digitalsampling.h"
#include "digitalsampling_internal.h"

// ----------------------------------------------------------------------------
//    devices to be used for digital sampling
//
const PIO pio = pio0;
const uint sm = 0;
const uint control_dma = 0;
const uint transfer_dma = 1;

char* errormessage;

char* digitalsampling_start(struct probe_controls* controls) {
    bus_ctrl_hw->priority = BUSCTRL_BUS_PRIORITY_DMA_W_BITS | BUSCTRL_BUS_PRIORITY_DMA_R_BITS;
    if (!(
            setuptransferbuffers(controls)
            && setupPIOandSM(controls)
            && setupDMAcontrollers(controls)
            && waitforstartevent(controls)
        )) {
        return errormessage;
    };
    pio_sm_set_enabled(pio, sm, true);
    dma_channel_start(control_dma);
    return NULL;
}

// ----------------------------------------------------------------------------
//   Data storage - data capture buffers, DMA control word buffers
//
#define BUFFERSPOWEROF2 2
//NUMBER_OF_BUFFERS= pow(2,BUFFERSPOWEROF2)
#define NUMBER_OF_BUFFERS 4 
// ring buffer mask used to search for a suitable start address for a DMA ring
// so  2 bits (word aligned) plus NUMBER_OF_BUFFERS size as a bit pattern
// so  0x03 for the word alignment and 0xc0 for a 4 buffer ring
#define RINGBASEMASK 0x0000000f
#define WORDSIZE 32
uint32_t buffer_size_words;
uint samplesperword;
uint usedbitsperword;
// the command list for use by the control DMA
// twice Number of BUFFERS to allow for space to get alignment for DMA ring
static uint32_t *commandlistbase[NUMBER_OF_BUFFERS*2];
static uint32_t **commandlist;
// the DMA buffers
static uint32_t *capture_bufs[NUMBER_OF_BUFFERS];

bool setuptransferbuffers(struct probe_controls* controls) {
    samplesperword = WORDSIZE/controls->pinwidth;
    usedbitsperword = samplesperword*controls->pinwidth;
    buffer_size_words = (controls->samplesize / samplesperword) / NUMBER_OF_BUFFERS;
    // dma & capture buffer setup
    if (controls->sampleendmode == BUFFER_FULL ) {
        commandlist = commandlistbase;
    } else {
        uint  lowbits = 1;
        commandlist = commandlistbase;
        for (int i = 0 ; i < NUMBER_OF_BUFFERS ; i++) {
            lowbits = ((uint32_t) commandlist)&RINGBASEMASK;
            if (lowbits == 0) {
                break;
            }
            commandlist++;
        }
        if (lowbits != 0 ) {
            errormessage = "can't align DMA ring buffer - system failure";
            return false;
        }
    }
    uint32_t **commandlistinsert = commandlist;
    for (int i = 0; i< NUMBER_OF_BUFFERS; i++) {
        capture_bufs[i]=malloc(buffer_size_words * sizeof(uint32_t));
        if ( capture_bufs[i] == NULL ) {
            errormessage = "could not obtain memory for sample storage";
            return false;
        }
        *commandlistinsert++ = capture_bufs[i];
    }
    if (controls->sampleendmode == BUFFER_FULL) {
        *commandlistinsert = NULL; // termination of the list - cause DMA to stop
    }
    return true;
}

uint irq0_count = 0;
bool dmafinished;

bool setupDMAcontrollers(struct probe_controls* controls) {
    dmafinished = false;
    // setup the control DMA
    dma_channel_config cc = dma_channel_get_default_config(control_dma);
    channel_config_set_read_increment(&cc, true);
    channel_config_set_write_increment(&cc, false);
    if (controls->sampleendmode != BUFFER_FULL) {
        channel_config_set_ring(&cc, false, BUFFERSPOWEROF2);
    }
    dma_channel_configure(control_dma, &cc,
        &dma_hw->ch[transfer_dma].al2_write_addr_trig,        // Destination pointer
        commandlist,      // Source pointer
        1, // Number of transfers 
        false
    );
    // setup the transfer DMA
    dma_channel_config ct = dma_channel_get_default_config(transfer_dma);
    channel_config_set_read_increment(&ct, false);
    channel_config_set_write_increment(&ct, true);
    channel_config_set_dreq(&ct, pio_get_dreq(pio, sm, false));
    channel_config_set_chain_to(&ct, control_dma);
    dma_channel_configure(transfer_dma, &ct,
        NULL,        // this will be added by the control dma
        &pio->rxf[sm],      // Source pointer
        buffer_size_words, // Number of transfers 
        false
    );
    //dma_channel_set_irq0_enabled(dmachannel,true);

    //irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
    //irq_set_enabled(DMA_IRQ_0, true);
    return true;
}

void dma_irq_handler() {
    dma_channel_acknowledge_irq0(irq0_count++);
}

void storage_waituntilcompleted(){
    dma_channel_wait_for_finish_blocking(0);
    dma_channel_wait_for_finish_blocking(1);
    dma_channel_wait_for_finish_blocking(2);
    dma_channel_wait_for_finish_blocking(3);
    dma_channel_wait_for_finish_blocking(4);
}

bool setupPIOandSM(struct probe_controls* controls) {
    uint16_t capture_prog_instr = pio_encode_in(controls->pinbase, controls->pinwidth);
    struct pio_program capture_prog = {
            .instructions = &capture_prog_instr,
            .length = 1,
            .origin = -1
    };
    uint offset = pio_add_program(pio, &capture_prog);
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_in_pins(&c, controls->pinbase);
    sm_config_set_wrap(&c, offset, offset);
    float div = (float) clock_get_hz(clk_sys)/controls->frequency;
    sm_config_set_clkdiv(&c, div);
    sm_config_set_in_shift(&c, false, true, usedbitsperword);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, false);
    pio_sm_clear_fifos(pio, sm);
    pio_sm_restart(pio, sm);
    return true;
}

bool waitforstartevent(struct probe_controls* controls) {
    pio_sm_exec(pio, sm, pio_encode_wait_gpio(controls->st_trigger==TRIGGER_ON_HIGH, controls->st_pin));
    return true;
}

// ========================================================================
//
//   Run length encoding - is how the sampled data is presented by the probe
//
// ========================================================================

// define max size of RLE text segment that is returned
#define MAXUSBTRANSFERBUFFERSIZE 72 

uint32_t maxcount;
uint spaceformaxcount;
uint maxlinelength;
void (*outputfunction)(const char *line);

char rlebuffer[MAXUSBTRANSFERBUFFERSIZE];
char* insertptr;
bool logic_level;
uint32_t count;

// ========================================================================
//
//  extracting data bits from dma buffers and creating run length encoded
//  data for each sample pin
//
// ========================================================================

void create_RLE_encoded_sample(struct probe_controls* controls, void (*outputfunction)(const char *line)){
    init_rle(6, 72, outputfunction);
    for(uint pinoffset = 0; pinoffset < controls->pinwidth; pinoffset++) {
        printf("# %i\n", pinoffset+controls->pinbase);
        insertptr = rlebuffer;
        count = 0;
        for (uint bufferno = 0; bufferno < NUMBER_OF_BUFFERS; bufferno++){
            for (uint wordno = 0; wordno < buffer_size_words ; wordno++) {
                uint32_t wordvalue = capture_bufs[bufferno][wordno];
                for (int bitcount = usedbitsperword -controls->pinwidth + pinoffset; bitcount >=0 ; bitcount-=controls->pinwidth) {
                    uint32_t mask = 1u<<bitcount;
                    rle_insertvalue((wordvalue&mask) > 0);
                }
            }
        }
        writetobuffer(); // flush final rle component into the buffer
        outputfunction(rlebuffer); // .. and write the buffer
    }
}

void init_rle(uint maxdigits, uint _maxlinelength, void (*_outputfunction)(const char *line)) {
    insertptr = rlebuffer;
    count = 0;
    spaceformaxcount = maxdigits + 2;
    maxcount = 0;
    while (maxdigits > 0 ) {
        maxcount = maxcount*10 + 9;
        maxdigits--;
    }
    maxlinelength = _maxlinelength;
    assert(maxlinelength <= MAXUSBTRANSFERBUFFERSIZE);
    outputfunction = _outputfunction;
}

void rle_insertvalue(bool logic_value) {
    if (count == 0) {
        logic_level =logic_value;
        count = 1;
    } else if (logic_value == logic_level) {
        count++;
        if (count == maxcount) {
            writetobuffer();
            count=0;
        }
    } else {
        writetobuffer();
        count = 1;
        logic_level = logic_value;
    }
}

void writetobuffer() {
    int n;
    if (insertptr -rlebuffer >= maxlinelength - spaceformaxcount ) {
       *insertptr = '\0';
        outputfunction(rlebuffer);
        insertptr = rlebuffer;
    }
    if (count == 1 ) {
        n = sprintf(insertptr, "%c", logic_level?'H':'L');
    } else {
        n= sprintf(insertptr, "%i%c", count,logic_level?'H':'L');
    }
    insertptr+=n;
}

// for testing only
char* get_linebuffer() {
    return rlebuffer;
}
