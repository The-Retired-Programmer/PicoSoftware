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

#define DMA_SPEED_DIVIDER 250
// dma word transfer speed is 125Mhz / 250  => 500 K words/sec
// 1280 sample size =>  10 words / buffer
// so rate = 50k buffers /sec or 20 micro secs per buffer
#define USECS_BUFFER 20
#define DELTA USECS_BUFFER/10
#define readdatainit  0xcccc0000
volatile uint32_t readdata;
struct probe_controls controls;

static bool test_digitalsampling_dma_stop_preactions(char *config) {
    readdata = readdatainit;
    struct probe_controls controls;
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
    dma_set_timer(0, 1, DMA_SPEED_DIVIDER);
    errormessage = setupDMAcontrollers(&controls, &readdata, 0x3b);
    if ( errormessage != NULL ) {
        fail(errormessage);
        return false;
    }
    dma_start();
    return true;
}

static void test_digitalsampling_dma_stop_postactions(uint expected_earliest_buffer, uint expected_buffers_used, char* expectedtrace) {
    struct sample_buffers *samplebuffers = getsamplebuffers();
    readdata = readdata & 0xff00ffff; // mark the buffer where the event occurs
    while (!samplebuffers->sampling_done); // wait for completion
    pass("transfer completed signalled");
    pass_if_equal_uint("first logical buffer", expected_earliest_buffer, samplebuffers->earliest_valid_buffer);
    pass_if_equal_uint("buffers filled", expected_buffers_used, samplebuffers->valid_buffer_count);
    for (uint i = 0; i < samplebuffers->valid_buffer_count ; i++) {
        uint bufferoffset = (i + samplebuffers->earliest_valid_buffer) % samplebuffers->number_of_buffers;
        printf("buffer(%i): first %x, last %x\n", bufferoffset, samplebuffers->buffers[bufferoffset][0],
            samplebuffers->buffers[bufferoffset][samplebuffers->buffer_size_words-1]);
    }
    pass_if_trace_equal(expectedtrace);
    teardown_dma();
    teardowntransferbuffers();
}

static void test_digitalsampling_dma_internals() {
    char *config = "g-19-1-19200-0-19-0-0-19-0-1-1280";
    if ( !test_digitalsampling_dma_stop_preactions(config)) return;
    test_digitalsampling_dma_stop_postactions(0, 4, "CCCCCT");
}

static void test_digitalsampling_dma_stop_now(char *config, uint32_t stop_delay, uint expected_earliest_buffer, uint expected_buffers_used, char* expectedtrace) {
    if ( !test_digitalsampling_dma_stop_preactions(config)) return;
    busy_wait_us_32(stop_delay);
    dma_stop();
    test_digitalsampling_dma_stop_postactions(expected_earliest_buffer, expected_buffers_used, expectedtrace);
}

static void test_digitalsampling_dma_stop_in_window(char *config, uint logicalwindow, uint expected_earliest_buffer, uint expected_buffers_used, char* expectedtrace) {
    if ( !test_digitalsampling_dma_stop_preactions(config)) return;
    busy_wait_us_32(USECS_BUFFER*6+DELTA); // a delay to allow the buffers to rotate
    dma_stop_where_now_is_in_window(logicalwindow);
    test_digitalsampling_dma_stop_postactions(expected_earliest_buffer, expected_buffers_used, expectedtrace);
}

static void test_digitalsampling_dma_stop_quick() {
    test_digitalsampling_dma_stop_now("g-19-1-19200-0-19-0-0-19-0-0-1280", USECS_BUFFER+DELTA, 0, 2, "CCSCT");
}

static void test_digitalsampling_dma_stop_long() {
    test_digitalsampling_dma_stop_now("g-19-1-19200-0-19-0-0-19-0-0-1280",USECS_BUFFER*6+DELTA, 3, 4, "CCCCCcST");
}

static void test_digitalsampling_dma_stop_w1() {
    test_digitalsampling_dma_stop_in_window("g-19-1-19200-0-19-0-0-19-0-2-1280",1, 2, 4, "CCCCCcW1T");
}

static void test_digitalsampling_dma_stop_w2() {
    test_digitalsampling_dma_stop_in_window("g-19-1-19200-0-19-0-0-19-0-3-1280",2, 1, 4, "CCCCCcW2T");
}

static void test_digitalsampling_dma_stop_w3() {
    test_digitalsampling_dma_stop_in_window("g-19-1-19200-0-19-0-0-19-0-4-1280",3, 0, 4, "CCCCCcW3T");
}

static void test_digitalsampling_dma_stop_w4() {
    test_digitalsampling_dma_stop_in_window("g-19-1-19200-0-19-0-0-19-0-5-1280",4, 3, 4, "CCCCCcW4T");
}

// =============================================================================
//
// module API
//
// =============================================================================

void test_dma_digitalsampling_init() {
    add_test("dma digitalsampling", "ud", test_digitalsampling_dma_internals);
    add_test("dma stop quick", "ud", test_digitalsampling_dma_stop_quick);
    add_test("dma stop long", "ud", test_digitalsampling_dma_stop_long);
    add_test("dma stop window1", "udw", test_digitalsampling_dma_stop_w1);
    add_test("dma stop window2", "udw", test_digitalsampling_dma_stop_w2);
    add_test("dma stop window3", "udw", test_digitalsampling_dma_stop_w3);
    add_test("dma stop window4", "udw", test_digitalsampling_dma_stop_w4);

}
