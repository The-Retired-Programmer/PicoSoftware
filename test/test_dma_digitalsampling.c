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

#include <stdlib.h>
#include "hardware/clocks.h"
#include "hardware/timer.h"
#include "ptest.h"
#include "../src/digitalsampling.h"
#include "../src/dma_digitalsampling.h"

#define readdatainit  0xcccc0000
volatile uint32_t readdata;
struct probe_controls controls;

static bool test_digitalsampling_dma_stop_preactions(char *config) {
    readdata = readdatainit;
    struct probe_controls controls;
    // 1024 sample size =>  8 words / buffer
    char* errormessage = parse_control_parameters(&controls,config);
    if ( errormessage != NULL ) {
        fail(errormessage);
        return false;
    }
    errormessage = setuptransferbuffers(&controls);
    if ( errormessage != NULL ) {
        fail(errormessage);
        return false;
    }
    dma_set_timer(0, 1, 256);
    errormessage = setupDMAcontrollers(&controls, &readdata, 0x3b);
    if ( errormessage != NULL ) {
        fail(errormessage);
        return false;
    }
    dma_start();
    return true;
}

static void test_digitalsampling_dma_stop_postactions() {
    struct sample_buffers *samplebuffers = getsamplebuffers();
    readdata = readdata & 0xff00ffff; // mark the buffer where the event occurs
    trace('w');
    while (!samplebuffers->sampling_done); // wait for completion
    trace('c');
    pass("transfer completed signalled");
    pass_if_equal_uint("buffers filled", 4, samplebuffers->valid_buffer_count);
    for (uint i = 0; i < samplebuffers->valid_buffer_count ; i++) {
        uint bufferoffset = (i + samplebuffers->earliest_valid_buffer) % samplebuffers->number_of_buffers;
        printf("buffer: first %x, last %x\n", samplebuffers->buffers[bufferoffset][0],
            samplebuffers->buffers[bufferoffset][samplebuffers->buffer_size_words-1]);
    }
    printf("Sample Buffers: start at %i; count is %i\n",
        samplebuffers->earliest_valid_buffer,
        samplebuffers->valid_buffer_count);
}

static void test_digitalsampling_dma_internals() {
    char *config = "g-16-1-19200-0-16-0-0-16-0-1-1024";
    if ( !test_digitalsampling_dma_stop_preactions(config)) return;
    test_digitalsampling_dma_stop_postactions();
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
    add_test("dma digitalsampling", "ud", test_digitalsampling_dma_internals);
    add_test("dma stop", "ud", test_digitalsampling_dma_stop);
    add_test("dma stop window1", "udw", test_digitalsampling_dma_stop_w1);
    add_test("dma stop window2", "udw", test_digitalsampling_dma_stop_w2);
    add_test("dma stop window3", "udw", test_digitalsampling_dma_stop_w3);
    add_test("dma stop window4", "udw", test_digitalsampling_dma_stop_w4);

}
