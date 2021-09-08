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

// -----------------------------------------------------------------------------
//
//  DMA DIGITALSAMPLING
//
//  transfer samples (normally from PIO FIFO) to a set of buffers in rotation
//
// there are two DMA channels involved:
//
// Control: has either a serial command buffer or a ring command buffer; transfers
// one word per cycle int the write register of the Transfer channel, so initiating
// a buffer sized transfer. ring mode is used normally (while waiting for some
// signal to terminate transfer at end of a buffer transfer), or if the simple
// BUFFER_FULL option is selected the transfer will complete once all buffers are
// full (so just a simple serial command buffer is used).  If ring mode is used
// the command data must be aligned to a simple power of 2 boundary.  IRQ
// callbacks can be enabled - each time the transfer channel has been restarted.
//
// Transfer: does the actual transfer from the input (normally the PIO) and
// transfers words into the buffer. IRQ callback can be enabled - on completion
// of final buffer transfer (ie when the control defines a NULL write address (trigger))
//
// -----------------------------------------------------------------------------
#include <stdlib.h>
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/structs/bus_ctrl.h"
#include "dma_digitalsampling.h"
#include "digitalsampling.h"

#define CONTROL_DMA_CHANNEL 0
#define TRANSFER_DMA_CHANNEL 1

#define BUFFERSPOWEROF2  2
//assert NUMBER_OF_BUFFERS == pow(2,BUFFERSPOWEROF2);
#define RINGBASEMASK 0x0000000f; // 2 bits (0xc) = 4 buffers aligment; 2 bits (0x3) = word alignment
//
// the command list for use by the control DMA
// twice Number of BUFFERS to allow for space to get alignment for DMA ring (or terminating NULL if simple)
//
static uint32_t *commandlistbase[4*2];
static uint32_t **commandlist;
static uint number_of_buffers;
void (*on_dma_irq0)();
void (*on_dma_irq1)();

static void dma_irq0_handler() {
    (*on_dma_irq0)();
    dma_channel_acknowledge_irq0(CONTROL_DMA_CHANNEL);
}

static void dma_irq1_handler() {
    (*on_dma_irq1)();
    dma_channel_acknowledge_irq1(TRANSFER_DMA_CHANNEL);
}

static void dma_stop_with_now_in_window(uint first_window_offset) {
    uint32_t **commandlist_read = dma_channel_get_read_addr(CONTROL_DMA_CHANNEL);
    uint clr_offset = commandlist_read - commandlist;
    uint stop_offset = (clr_offset + first_window_offset) % number_of_buffers;   
    commandlist[stop_offset] = NULL;
}

// =============================================================================
//
// module API
//
// =============================================================================

char* setupDMAcontrollers(struct probe_controls* controls, const volatile uint32_t *readaddress, uint dreq) {
    struct sample_buffers samplebuffers = getsamplebuffers();
    number_of_buffers = samplebuffers.number_of_buffers;
    //
    // command DMA buffer set
    //
    if (controls->sampleendmode == BUFFER_FULL ) {
        commandlist = commandlistbase;
    } else {
        uint  lowbits = 1;
        commandlist = commandlistbase;
        for (int i = 0 ; i < number_of_buffers ; i++) {
            lowbits = ((uint32_t) commandlist)&RINGBASEMASK;
            if (lowbits == 0) {
                break;
            }
            commandlist++;
        }
        if (lowbits != 0 ) {
            return "can't align DMA ring buffer - system failure";
        }
    }
    // commandlist now points at the start of the command buffer
    uint32_t **commandlistinsert = commandlist;
    for (int i = 0; i< number_of_buffers; i++) {
        *commandlistinsert++ = samplebuffers.buffers[i];
    }
    if (controls->sampleendmode == BUFFER_FULL) {
        *commandlistinsert = NULL; // termination of the list - cause DMA to stop
    }
    //
    // setup the control DMA
    //
    dma_channel_config cc = dma_channel_get_default_config(CONTROL_DMA_CHANNEL);
    channel_config_set_read_increment(&cc, true);
    channel_config_set_write_increment(&cc, false);
    if (controls->sampleendmode != BUFFER_FULL) {
        channel_config_set_ring(&cc, false, BUFFERSPOWEROF2 + 2 );
    }
    dma_channel_configure(CONTROL_DMA_CHANNEL, &cc,
        &dma_hw->ch[TRANSFER_DMA_CHANNEL].al2_write_addr_trig, // Destination pointer
        commandlist,                                   // Source pointer
        1, // Number of transfers 
        false
    );
    //
    // setup the transfer DMA
    //
    dma_channel_config ct = dma_channel_get_default_config(TRANSFER_DMA_CHANNEL);
    channel_config_set_read_increment(&ct, false);
    channel_config_set_write_increment(&ct, true);
    channel_config_set_dreq(&ct, dreq);
    channel_config_set_chain_to(&ct, CONTROL_DMA_CHANNEL);
    channel_config_set_irq_quiet (&ct, true);
    dma_channel_configure(TRANSFER_DMA_CHANNEL, &ct,
        NULL,        // this will be added by the control dma (write address)
        readaddress,      // Source pointer
        samplebuffers.buffer_size_words, // Number of transfers 
        false
    );
    return NULL;
}

void dma_after_every_control(void (*callback)()) {
    on_dma_irq0 = callback;
    dma_channel_set_irq0_enabled(CONTROL_DMA_CHANNEL,true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq0_handler);
    irq_set_enabled(DMA_IRQ_0, true);
}

void dma_on_completed(void (*callback)()) {
    on_dma_irq1 = callback;
    dma_channel_set_irq1_enabled(TRANSFER_DMA_CHANNEL,true);
    irq_set_exclusive_handler(DMA_IRQ_1, dma_irq1_handler);
    irq_set_enabled(DMA_IRQ_1, true);
}

void dma_to_have_bus_priority() {
    bus_ctrl_hw->priority = BUSCTRL_BUS_PRIORITY_DMA_W_BITS | BUSCTRL_BUS_PRIORITY_DMA_R_BITS;
}

void dma_start() {
    dma_channel_start(CONTROL_DMA_CHANNEL);
}

// stop at end of this buffer
void dma_stop() {
    uint32_t **commandlistinsert = commandlist;
    for (int i = 0; i< number_of_buffers; i++) {
        *commandlistinsert++ = NULL;
    }    
}

void dma_stop_with_now_in_window1() {
    dma_stop_with_now_in_window(3);
}

void dma_stop_with_now_in_window2() {
    dma_stop_with_now_in_window(2);
}

void dma_stop_with_now_in_window3() {
    dma_stop_with_now_in_window(1);
}

void dma_stop_with_now_in_window4() {
    dma_stop_with_now_in_window(0);
}
