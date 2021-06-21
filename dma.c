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

// DMA management - to copy all PIO generated capture data to memory buffers

#include "pio.h"
#include "hardware/dma.h"
#include "hardware/structs/bus_ctrl.h"

const uint DMA_CHAN = 0;

void dma_init() {
    // Grant high bus priority to the DMA, so it can shove the processors out
    // of the way. This should only be needed if you are pushing things up to
    // >16bits/clk here, i.e. if you need to saturate the bus completely.
    bus_ctrl_hw->priority = BUSCTRL_BUS_PRIORITY_DMA_W_BITS | BUSCTRL_BUS_PRIORITY_DMA_R_BITS;
}

void dma_arm(uint32_t *capture_buf, size_t capture_size_words) {
    dma_channel_config c = dma_channel_get_default_config(DMA_CHAN);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    channel_config_set_dreq(&c, get_read_dreq());
    dma_channel_configure(DMA_CHAN, &c,
        capture_buf,        // Destination pointer
        getPIOsource(),      // Source pointer
        capture_size_words, // Number of transfers
        true                // Start immediately
    );
}

void dma_waituntilcompleted() {
    dma_channel_wait_for_finish_blocking(DMA_CHAN);
}

