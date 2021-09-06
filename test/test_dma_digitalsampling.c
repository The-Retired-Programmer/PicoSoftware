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
// test - logic analyser/probe_controls
//

#include <stdlib.h>
#include "hardware/clocks.h"
#include "hardware/timer.h"
#include "ptest.h"
#include "../src/digitalsampling.h"
#include "../src/dma_digitalsampling.h"

#define readdatainit  0xcccc0000
volatile uint32_t readdata = readdatainit;
volatile uint dma_buffer_fills = 0 ;
volatile bool dma_completed = false;
volatile uint dma_completed_count = 0;

static void dma_buffer_callback() {
    readdata+=1;
    trace('0');
    dma_buffer_fills++;
}

static void dma_transfer_finished_callback() {
     trace('1');
    dma_completed = true;
}

static void test_digitalsampling_dma_internals() {
    dma_buffer_fills = 0;
    readdata = readdatainit;
    dma_completed = false;
    dma_completed_count = 0;
    struct probe_controls controls;
    // 1024 sample size means  8 words / buffer
    char* res = parse_control_parameters(&controls,"g-16-1-19200-0-16-0-0-16-0-1-1024"); // will only use samplesize and pin_width
    if ( res != NULL ) {
        fail(res);
        return;
    }
    res = setuptransferbuffers(&controls);
    if ( res != NULL ) {
        fail(res);
        return;
    }
    dma_set_timer(0, 1, 256);
    res = setupDMAcontrollers(&controls, &readdata, 0x3b); //
    if ( res != NULL ) {
        fail(res);
        return;
    }
    dma_to_have_bus_priority();
    dma_after_every_control(dma_buffer_callback);
    dma_on_completed(dma_transfer_finished_callback);
    dma_start();
    while (!dma_completed);
    pass("transfer completed signalled");
    readdata = readdatainit;
    pass_if_equal_uint("control count", 5, dma_buffer_fills);
}

static void test_digitalsampling_dma_stop() {
    dma_buffer_fills = 0;
    readdata = readdatainit;
    dma_completed = false;
    struct probe_controls controls;
    // 1024 sample size =>  8 words / buffer
    char* res = parse_control_parameters(&controls,"g-16-1-19200-0-16-0-0-16-0-0-1024"); // will only use samplesize and pin_width
    if ( res != NULL ) {
        fail(res);
        return;
    }
    res = setuptransferbuffers(&controls);
    if ( res != NULL ) {
        fail(res);
        return;
    }
    dma_set_timer(0, 1, 256);
    res = setupDMAcontrollers(&controls, &readdata, 0x3b);
    if ( res != NULL ) {
        fail(res);
        return;
    }
    dma_to_have_bus_priority();
    dma_after_every_control(dma_buffer_callback);
    dma_on_completed(dma_transfer_finished_callback);
    dma_start();
    // a delay to allow the buffers to rotate
    busy_wait_us_32(13*13);
    trace('s');
    dma_stop();
    trace('w');
    while (!dma_completed); // wait for completion
    trace('c');
    pass("transfer completed signalled");
    pass_if_greaterthan_uint("control count", 5, dma_buffer_fills);
}

// =============================================================================
//
// module API
//
// =============================================================================

void test_dma_digitalsampling_init() {
    add_test("dma digitalsampling", "dma", test_digitalsampling_dma_internals);
    add_test("dma stop", "dma_stop", test_digitalsampling_dma_stop);
}
