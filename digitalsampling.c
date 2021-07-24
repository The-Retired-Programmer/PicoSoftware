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

const PIO pio = pio0;
const uint sm = 0;

// data buffers - chained to form the storage
#define NUMBER_OF_BUFFERS 5
#define WORDSIZE 32

// buffer(s) to hold the captured data 

uint32_t *capture_bufs[NUMBER_OF_BUFFERS];
uint32_t buffer_size_words;
uint irq0_count = 0;
bool dmafinished;

void config_dma_channel(uint dmachannel, uint32_t *buffer, uint transfersizewords, uint nextdmachannel);
void dma_irq_handler();

char* digitalsampling_start(struct probe_controls* controls) {
    // calculate any setup parameters - no validation here - it's been done prior.
    uint samplesperword = WORDSIZE/controls->pinwidth;
    uint usedbitsperword = samplesperword*controls->pinwidth;
    uint32_t capture_size_words = controls->samplesize / samplesperword;
    buffer_size_words = capture_size_words / NUMBER_OF_BUFFERS;
    // dma & capture buffer setup
    for (int i = 0; i< NUMBER_OF_BUFFERS; i++) {
        capture_bufs[i]=malloc(buffer_size_words * sizeof(uint32_t));
        if ( capture_bufs[i] == NULL ) {
            return "could not obtain buffer space for samples";
        }
    }
    bus_ctrl_hw->priority = BUSCTRL_BUS_PRIORITY_DMA_W_BITS | BUSCTRL_BUS_PRIORITY_DMA_R_BITS;
    // pio setup
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
    sm_config_set_in_shift(&c, true, true, usedbitsperword);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    pio_sm_init(pio, sm, offset, &c);

    dmafinished = false;
    config_dma_channel(0,capture_bufs[0], buffer_size_words, 1);
    config_dma_channel(1,capture_bufs[1], buffer_size_words, 2);
    config_dma_channel(2,capture_bufs[2], buffer_size_words, 3);
    config_dma_channel(3,capture_bufs[3], buffer_size_words, 4);
    config_dma_channel(4,capture_bufs[4], buffer_size_words, 4);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
    irq_set_enabled(DMA_IRQ_0, true);
    //
    pio_sm_set_enabled(pio, sm, false);
    pio_sm_clear_fifos(pio, sm);
    pio_sm_restart(pio, sm);
    pio_sm_exec(pio, sm, pio_encode_wait_gpio(controls->st_trigger==TRIGGER_ON_HIGH, controls->st_pin));
    pio_sm_set_enabled(pio, sm, true);
    //
    dma_channel_start(0);
    return NULL;
}

void config_dma_channel(uint dmachannel, uint32_t *buffer, uint transfersizewords, uint nextdmachannel) {
    dma_channel_config c = dma_channel_get_default_config(dmachannel);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    channel_config_set_dreq(&c, pio_get_dreq(pio, sm, false));
    channel_config_set_chain_to(&c, nextdmachannel);
    dma_channel_configure(dmachannel, &c,
        buffer,        // Destination pointer
        &pio->rxf[sm],      // Source pointer
        transfersizewords, // Number of transfers 
        false
    );
    dma_channel_set_irq0_enabled(dmachannel,true);
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

// ========================================================================
//
//   Run length encoding - is how the sampled data is presented by the probe
//
// ========================================================================

// define max size of RLE text segment that is returned
#define USBTRANSFERBUFFERSIZE 128 

int count = 0;
bool logic_level;

char rlebuffer[USBTRANSFERBUFFERSIZE];
char* insertptr = rlebuffer;

void writetobuffer(int(*writesegment)(const char*)) {
    int n;
    if (insertptr -rlebuffer > USBTRANSFERBUFFERSIZE - 8 ) {
        writesegment(rlebuffer);
        insertptr = rlebuffer;
    }
    if (count == 1 ) {
        n = sprintf(insertptr, "%c", logic_level?'H':'L');
    } else {
        n= sprintf(insertptr, "%i%c", count,logic_level?'H':'L');
    }
    insertptr+=n;
}

void rle_add_point(bool logic_value, int(*writesegment)(const char*)) {
    if (count == 0) {
        logic_level =logic_value;
        count = 1;
    } else if (logic_value == logic_level) {
        count++;
        if (count == 999999) {
            writetobuffer(writesegment);
            count=0;
        }
    } else {
        writetobuffer(writesegment);
        count = 1;
        logic_level = logic_value;
    }
}

// ========================================================================
//
//  extracting data bits from dma buffers and creating run length encoded
//  data for each sample pin
//
// ========================================================================

void create_RLE_encoded_sample(struct probe_controls* controls, int(*writesegment)(const char*)){
    insertptr = rlebuffer;
    assert(controls->pinwidth ==1 );  // base implementation
    for (uint bufferno = 0; bufferno < NUMBER_OF_BUFFERS; bufferno++){
        for (uint wordno = 0; wordno < buffer_size_words ; wordno++) {
            uint32_t wordvalue = capture_bufs[bufferno][wordno];
            for (uint bitcount = 0; bitcount < 32 ; bitcount++) {
                uint32_t mask = 1u<<bitcount;
                rle_add_point((wordvalue&mask) > 0,writesegment );
            }
        }
    }
    writetobuffer(writesegment); // flush final rle component
    writesegment(rlebuffer);
}
