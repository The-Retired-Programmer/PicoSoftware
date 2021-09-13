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
struct probe_controls controls;

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
    struct sample_buffers *samplebuffers = getsamplebuffers();
    for (uint i = 0; i < samplebuffers->number_of_buffers ; i++) {
        printf("buffer: first %x, last %x\n", samplebuffers->buffers[i][0],
            samplebuffers->buffers[i][samplebuffers->buffer_size_words-1]);
    }
    printf("Sample Buffers: start at %i; count is %i\n",
        samplebuffers->earliest_valid_buffer,
        samplebuffers->valid_buffer_count);
}

static bool test_digitalsampling_dma_stop_preactions(char *config) {
    dma_buffer_fills = 0;
    readdata = readdatainit;
    dma_completed = false;
    struct probe_controls controls;
    // 1024 sample size =>  8 words / buffer
    char* res = parse_control_parameters(&controls,config); // will only use samplesize and pin_width
    if ( res != NULL ) {
        fail(res);
        return false;
    }
    res = setuptransferbuffers(&controls);
    if ( res != NULL ) {
        fail(res);
        return false;
    }
    dma_set_timer(0, 1, 256);
    res = setupDMAcontrollers(&controls, &readdata, 0x3b);
    if ( res != NULL ) {
        fail(res);
        return false;
    }
    dma_to_have_bus_priority();
    dma_after_every_control(dma_buffer_callback);
    dma_on_completed(dma_transfer_finished_callback);
    dma_start();
    return true;
}

static void test_digitalsampling_dma_stop_postactions() {
    readdata = readdata & 0xff00ffff; // mark the buffer where the event occurs
    trace('w');
    while (!dma_completed); // wait for completion
    trace('c');
    pass("transfer completed signalled");
    pass_if_greaterthan_uint("control count", 5, dma_buffer_fills);
    struct sample_buffers *samplebuffers = getsamplebuffers();
    for (uint i = 0; i < samplebuffers->number_of_buffers ; i++) {
        printf("buffer: first %x, last %x\n", samplebuffers->buffers[i][0],
            samplebuffers->buffers[i][samplebuffers->buffer_size_words-1]);
    }
    printf("Sample Buffers: start at %i; count is %i\n",
        samplebuffers->earliest_valid_buffer,
        samplebuffers->valid_buffer_count);
}

static void test_digitalsampling_dma_stop_now(char *config) {
    if ( !test_digitalsampling_dma_stop_preactions(config)) return;
    busy_wait_us_32(13*13); // a delay to allow the buffers to rotate
    trace('s');
    dma_stop();
    test_digitalsampling_dma_stop_postactions();
}

static void test_digitalsampling_dma_stop_in_window(char *config, uint logicalwindow) {
    if ( !test_digitalsampling_dma_stop_preactions(config)) return;
    busy_wait_us_32(13*13); // a delay to allow the buffers to rotate
    trace('s');
    dma_stop_where_now_is_in_window(logicalwindow);
    test_digitalsampling_dma_stop_postactions();
}

static void test_digitalsampling_dma_stop() {
    test_digitalsampling_dma_stop_now("g-16-1-19200-0-16-0-0-16-0-0-1024");
}

static void test_digitalsampling_dma_stop_w1() {
    test_digitalsampling_dma_stop_in_window("g-16-1-19200-0-16-0-0-16-0-2-1024",1);
}

static void test_digitalsampling_dma_stop_w2() {
    test_digitalsampling_dma_stop_in_window("g-16-1-19200-0-16-0-0-16-0-3-1024",2);
}

static void test_digitalsampling_dma_stop_w3() {
    test_digitalsampling_dma_stop_in_window("g-16-1-19200-0-16-0-0-16-0-4-1024",3);
}

static void test_digitalsampling_dma_stop_w4() {
    test_digitalsampling_dma_stop_in_window("g-16-1-19200-0-16-0-0-16-0-5-1024",4);
}

// =============================================================================
//
// module API
//
// =============================================================================

void test_dma_digitalsampling_init() {
    add_test("dma digitalsampling", "dma", test_digitalsampling_dma_internals);
    add_test("dma stop", "dma_stop", test_digitalsampling_dma_stop);
    add_test("dma stop window1", "dma_stop_w1", test_digitalsampling_dma_stop_w1);
    add_test("dma stop window2", "dma_stop_w2", test_digitalsampling_dma_stop_w2);
    add_test("dma stop window3", "dma_stop_w3", test_digitalsampling_dma_stop_w3);
    add_test("dma stop window4", "dma_stop_w4", test_digitalsampling_dma_stop_w4);

}
