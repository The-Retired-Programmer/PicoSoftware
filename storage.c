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
#include "hardware/structs/bus_ctrl.h"

void config_dma_channel(uint dmachannel, uint32_t *buffer, uint transfersizewords, uint nextdmachannel);
void dma_irq_handler();

// data buffers - chained to form the storage
#define NUMBER_OF_BUFFERS 3
#define BUFFERSIZEWORDS 3

// buffer(s) to hold the captured data 

uint32_t *capture_bufs[NUMBER_OF_BUFFERS];
uint capture_size_words;
uint irq0_count = 0;
bool dmafinished;

uint pinbase;
uint pincount;
uint sample_size;

void storage_init(uint samplesize, uint firstpin, int pinspersample) {
    capture_size_words = (samplesize * pinspersample + 31) / 32;
    sample_size = samplesize;
    pinbase = firstpin;
    pincount = pinspersample;

    for (int i = 0; i< NUMBER_OF_BUFFERS; i++) {  // looks like we can get total of 64KBytes easily
        capture_bufs[i]=malloc(BUFFERSIZEWORDS* sizeof(uint32_t));
        hard_assert(capture_bufs[i]);
    }
    // Grant high bus priority to the DMA, so it can shove the processors out
    // of the way. This should only be needed if you are pushing things up to
    // >16bits/clk here, i.e. if you need to saturate the bus completely.
    bus_ctrl_hw->priority = BUSCTRL_BUS_PRIORITY_DMA_W_BITS | BUSCTRL_BUS_PRIORITY_DMA_R_BITS;
}

void storage_arm() {
    dmafinished = false;
    config_dma_channel(0,capture_bufs[0], 1, 1);
    config_dma_channel(1,capture_bufs[1], 1, 2);
    config_dma_channel(2,capture_bufs[2], 1, 2);
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
}

uint32_t *get_capturebuf(uint logicalindex) {
    return capture_bufs[logicalindex];
}

uint get_capturebuf_size() {
    return 1;
}

uint get_bufs_count() {
    return 3;
}

// ========================================================================
//
//   Run length encoding section
//
// ========================================================================

struct rleitem {
    struct rleitem *next;
    int count;
    bool logic_level;
};

struct rleitem *head;
struct rleitem *tail;
char* res;

struct rleitem *getnewitem(bool logic_level) {
    struct rleitem *item = malloc(sizeof(struct rleitem));
    item->next = NULL;
    item->count = 1;
    item->logic_level = logic_level;
    return item;
}

void rle_init() {
    head = NULL;
    tail = NULL;
    res = malloc(sample_size+1);
}

void rle_add_point(bool logic_level) {
    if (head == NULL) {
        head = getnewitem(logic_level);
        tail = head;
    } else if (logic_level == tail->logic_level) {
        (tail->count)++;
    } else {
        struct rleitem *newitem = getnewitem(logic_level);
        tail->next=newitem;
        tail = newitem;
    }
}

char *rle_encode_as_string() {
    struct rleitem *next = head;
    *res = '\0';
    while (next != NULL) {
        char rleelement[10];
        sprintf(rleelement, "%i%c", next->count,next->logic_level?'+':'-');
        strcat(res, rleelement);
        next = next->next;
    }
    return res;
}

void rle_free() {
    struct rleitem *item = head;
    while (item != NULL) {
        struct rleitem *nextitem = item->next;
        free(item);
        item = nextitem;
    }
    free(res);
}

// ========================================================================
//
//  extracting data bits from dma buffers and creating run length encoded
//  data for each ssmaple pin
//
// ========================================================================

char *get_RLE_encoded_sample(uint pin) {
    rle_init();
    assert(pincount ==1 );  // base implementation
    for (uint bufferno = 0; bufferno < NUMBER_OF_BUFFERS; bufferno++){
        for (uint wordno = 0; wordno < 1 ; wordno++) {
            uint32_t wordvalue = capture_bufs[bufferno][wordno];
            for (uint bitcount = 0; bitcount < 32 ; bitcount++) {
                uint32_t mask = 1u<<bitcount;
                rle_add_point((wordvalue&mask) > 0);
            }
        }
    }
    char *rlestring = rle_encode_as_string();
    rle_free();
    return rlestring;
}



