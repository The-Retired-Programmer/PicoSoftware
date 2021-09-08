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

#ifndef _DMA_DIGITALSAMPLING_H
#define _DMA_DIGITALSAMPLING_H

#include "probe_controls.h"
#include "hardware/regs/dma.h"
#include "hardware/structs/dma.h"
#include "hardware/dma.h"

char* setupDMAcontrollers(struct probe_controls* controls, const volatile uint32_t *readaddress, uint dreq);

void dma_after_every_control(void (*callback)());

void dma_on_completed(void (*callback)());

void dma_to_have_bus_priority();

void dma_start();

void dma_stop();

void dma_stop_with_now_in_window1();

void dma_stop_with_now_in_window2();

void dma_stop_with_now_in_window3();

void dma_stop_with_now_in_window4();

static inline void dma_set_timer(uint timer_num, uint xval, uint yval) {
    check_hw_layout(dma_hw_t, timer[0], DMA_TIMER0_OFFSET);
    dma_hw->timer[timer_num] = (xval << DMA_TIMER0_X_LSB | (yval & DMA_TIMER0_Y_BITS));;
}

static inline uint32_t** dma_channel_get_read_addr(uint channel) {
    return (uint32_t**) dma_channel_hw_addr(channel)->read_addr;
}

#endif
