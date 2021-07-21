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
// storage manager
//

#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "pio.h"
#include "probe_controls.h"
#include "hardware/structs/bus_ctrl.h"

void config_dma_channel(uint dmachannel, uint32_t *buffer, uint transfersizewords, uint nextdmachannel);
void dma_irq_handler();

// data buffers - chained to form the storage
#define NUMBER_OF_BUFFERS 5
#define BUFFERSIZEWORDS 5
#define USBTRANSFERBUFFERSIZE 128

// buffer(s) to hold the captured data 

uint32_t *capture_bufs[NUMBER_OF_BUFFERS];
uint capture_size_words;
uint irq0_count = 0;
bool dmafinished;

struct probe_controls* controls;

void storage_init(struct probe_controls* probecontrols) {
    controls = probecontrols;
    capture_size_words = (controls->samplesize * controls->pinwidth + 31) / 32;

    for (int i = 0; i< NUMBER_OF_BUFFERS; i++) {  // looks like we can get total of 64KBytes easily
        capture_bufs[i]=malloc(BUFFERSIZEWORDS* sizeof(uint32_t));
        hard_assert(capture_bufs[i]);
    }
    // Grant high bus priority to the DMA
    bus_ctrl_hw->priority = BUSCTRL_BUS_PRIORITY_DMA_W_BITS | BUSCTRL_BUS_PRIORITY_DMA_R_BITS;
}

void storage_arm() {
    dmafinished = false;
    config_dma_channel(0,capture_bufs[0], BUFFERSIZEWORDS, 1);
    config_dma_channel(1,capture_bufs[1], BUFFERSIZEWORDS, 2);
    config_dma_channel(2,capture_bufs[2], BUFFERSIZEWORDS, 3);
    config_dma_channel(3,capture_bufs[3], BUFFERSIZEWORDS, 4);
    config_dma_channel(4,capture_bufs[4], BUFFERSIZEWORDS, 4);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
    irq_set_enabled(DMA_IRQ_0, true);
    dma_channel_start(0);
}

void config_dma_channel(uint dmachannel, uint32_t *buffer, uint transfersizewords, uint nextdmachannel) {
    dma_channel_config c = dma_channel_get_default_config(dmachannel);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    channel_config_set_dreq(&c, get_read_dreq());
    channel_config_set_chain_to(&c, nextdmachannel);
    dma_channel_configure(dmachannel, &c,
        buffer,        // Destination pointer
        getPIOsource(),      // Source pointer
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

uint32_t *get_capturebuf(uint logicalindex) {
    return capture_bufs[logicalindex];
}

uint get_capturebuf_size() {
    return BUFFERSIZEWORDS;
}

uint get_bufs_count() {
    return NUMBER_OF_BUFFERS;
}

// ========================================================================
//
//   Run length encoding section
//
// ========================================================================

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

void create_RLE_encoded_sample(uint pin, int(*writesegment)(const char*)){
    insertptr = rlebuffer;
    assert(controls->pinwidth ==1 );  // base implementation
    for (uint bufferno = 0; bufferno < NUMBER_OF_BUFFERS; bufferno++){
        for (uint wordno = 0; wordno < 1 ; wordno++) {
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



