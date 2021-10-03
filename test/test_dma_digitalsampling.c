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

static bool pre_actions(char *config) {
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

static void test_first_and_last_buffer_values(char *id, struct sample_buffers *samplebuffers, uint bufferoffset, uint32_t expected_first, uint32_t expected_last) {
    pass_if_equal_uint32x(id, expected_first, samplebuffers->buffers[bufferoffset][0]);
    pass_if_equal_uint32x(id, expected_last, samplebuffers->buffers[bufferoffset][samplebuffers->buffer_size_words-1]);
}

static void post_actions(uint expected_earliest_buffer, uint expected_buffers_used, uint marked_logical_buffer, char* expectedtrace) {
    struct sample_buffers *samplebuffers = getsamplebuffers();
    readdata = readdata & 0xff00ffff; // mark the buffer where the event occurs
    while (!samplebuffers->sampling_done); // wait for completion
    pass("transfer completed signalled");
    pass_if_equal_uint("first logical buffer", expected_earliest_buffer, samplebuffers->earliest_valid_buffer);
    pass_if_equal_uint("buffers filled", expected_buffers_used, samplebuffers->valid_buffer_count);
    for (uint i = 0; i < samplebuffers->valid_buffer_count ; i++) {
        uint bufferoffset = (i + samplebuffers->earliest_valid_buffer) % samplebuffers->number_of_buffers;
        uint logical_buffer_offset = marked_logical_buffer -1;
        if (i < logical_buffer_offset) {
           test_first_and_last_buffer_values("values in previous buffers",samplebuffers, bufferoffset, 0xcccc0000, 0xcccc0000);
        } else if (i == logical_buffer_offset) {
            test_first_and_last_buffer_values("values in marked buffer", samplebuffers, bufferoffset, 0xcccc0000, 0xcc000000); 
        } else { // i > logical_buffer_offset
            test_first_and_last_buffer_values("values in following buffers",samplebuffers, bufferoffset, 0xcc000000, 0xcc000000);
        }
//        printf("buffer(%i): first %x, last %x\n", bufferoffset, samplebuffers->buffers[bufferoffset][0],
//            samplebuffers->buffers[bufferoffset][samplebuffers->buffer_size_words-1]);
    }
    teardown_dma();
    teardowntransferbuffers();
    pass_if_trace_equal(expectedtrace);
}

static void test_buffer_full() {
    char *config = "g-19-1-19200-0-19-0-0-19-0-1-1280";
    if ( !pre_actions(config)) return;
    post_actions(0, 4, 1, "CCCCCT");
}

static void test_manual_stop(char *config, uint32_t stop_delay, uint expected_earliest_buffer, uint expected_buffers_used, uint marked_logical_buffer, char* expectedtrace) {
    if ( !pre_actions(config)) return;
    busy_wait_us_32(stop_delay);
    dma_stop();
    post_actions(expected_earliest_buffer, expected_buffers_used, marked_logical_buffer, expectedtrace);
}

static void test_window_stop(char *config, uint32_t stop_delay,  uint logicalwindow, uint expected_earliest_buffer, uint expected_buffers_used, uint marked_logical_buffer, char* expectedtrace) {
    if ( !pre_actions(config)) return;
    busy_wait_us_32(stop_delay);
    dma_stop_where_now_is_in_window(logicalwindow);
    post_actions(expected_earliest_buffer, expected_buffers_used, marked_logical_buffer, expectedtrace);
}

static void test_manual_stop_quick() {
    test_manual_stop("g-19-1-19200-0-19-0-0-19-0-0-1280", USECS_BUFFER+DELTA, 0, 2, 2, "CCSCT");
}

static void test_manual_stop_long() {
    test_manual_stop("g-19-1-19200-0-19-0-0-19-0-0-1280",USECS_BUFFER*6+DELTA, 3, 4, 4, "CCCCCCCSCT");
}

static void test_window_stop_1() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-2-1280",USECS_BUFFER*6+DELTA,1, 2, 4, 1, "CCCCCCCW1CCCCT");
}

static void test_window_stop_1_quick() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-2-1280",DELTA,1, 0, 4, 1, "CW1CCCCT");
}

static void test_window_stop_1_onestep() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-2-1280",USECS_BUFFER+DELTA,1, 1, 4, 1, "CCW1CCCCT");
}

static void test_window_stop_1_twostep() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-2-1280",USECS_BUFFER*2+DELTA,1, 2, 4, 1, "CCCW1CCCCT");
}

static void test_window_stop_1_threestep() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-2-1280",USECS_BUFFER*3+DELTA,1, 3, 4, 1, "CCCCW1CCCCT");
}

static void test_window_stop_1_fourstep() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-2-1280",USECS_BUFFER*4+DELTA,1, 0, 4, 1, "CCCCCW1CCCCT");
}

static void test_window_stop_2() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-3-1280",USECS_BUFFER*6+DELTA,2, 1, 4, 2, "CCCCCCCW2CCCT");
}

static void test_window_stop_2_quick() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-3-1280",DELTA,2, 0, 4, 1, "CW2CCCCT");
}

static void test_window_stop_2_onestep() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-3-1280",USECS_BUFFER+DELTA,2, 0, 4, 2, "CCW2CCCT");
}

static void test_window_stop_2_twostep() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-3-1280",USECS_BUFFER*2+DELTA,2, 1, 4, 2, "CCCW2CCCT");
}

static void test_window_stop_2_threestep() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-3-1280",USECS_BUFFER*3+DELTA,2, 2, 4, 2, "CCCCW2CCCT");
}

static void test_window_stop_2_fourstep() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-3-1280",USECS_BUFFER*4+DELTA,2, 3, 4, 2, "CCCCCW2CCCT");
}

static void test_window_stop_3() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-4-1280",USECS_BUFFER*6+DELTA,3, 0, 4, 3, "CCCCCCCW3CCT");
}

static void test_window_stop_3_quick() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-4-1280",DELTA,3, 0, 4, 1, "CW3CCCCT");
}

static void test_window_stop_3_onestep() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-4-1280",USECS_BUFFER+DELTA,3, 0, 4, 2, "CCW3CCCT");
}

static void test_window_stop_3_twostep() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-4-1280",USECS_BUFFER*2+DELTA,3, 0, 4, 3, "CCCW3CCT");
}

static void test_window_stop_3_threestep() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-4-1280",USECS_BUFFER*3+DELTA,3, 1, 4, 3, "CCCCW3CCT");
}

static void test_window_stop_3_fourstep() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-4-1280",USECS_BUFFER*4+DELTA,3, 2, 4, 3, "CCCCCW3CCT");
}

static void test_window_stop_4() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-5-1280",USECS_BUFFER*6+DELTA,4, 3, 4, 4, "CCCCCCCW4CT");
}

static void test_window_stop_4_quick() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-5-1280",DELTA,4, 0, 4, 1, "CW4CCCCT");
}

static void test_window_stop_4_onestep() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-5-1280",USECS_BUFFER+DELTA,4, 0, 4, 2, "CCW4CCCT");
}

static void test_window_stop_4_twostep() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-5-1280",USECS_BUFFER*2+DELTA,4, 0, 4, 3, "CCCW4CCT");
}

static void test_window_stop_4_threestep() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-5-1280",USECS_BUFFER*3+DELTA,4, 0, 4, 4, "CCCCW4CT");
}

static void test_window_stop_4_fourstep() {
    test_window_stop("g-19-1-19200-0-19-0-0-19-0-5-1280",USECS_BUFFER*4+DELTA,4, 1, 4, 4, "CCCCCW4CT");
}

// =============================================================================
//
// module API
//
// =============================================================================

void test_dma_digitalsampling_init() {
    add_test("dma buffer full", "ud", test_buffer_full);
    add_test("dma stop quick", "ud", test_manual_stop_quick);
    add_test("dma stop long", "ud", test_manual_stop_long);
    add_test("dma stop window1", "udw", test_window_stop_1);
    add_test("dma stop window2", "udw", test_window_stop_2);
    add_test("dma stop window3", "udw", test_window_stop_3);
    add_test("dma stop window4", "udw", test_window_stop_4);
    add_test("dma stop window1 quick", "udw", test_window_stop_1_quick);
    add_test("dma stop window1 one step", "udw", test_window_stop_1_onestep);
    add_test("dma stop window1 two step", "udw", test_window_stop_1_twostep);
    add_test("dma stop window1 three step", "udw", test_window_stop_1_threestep);
    add_test("dma stop window1 four step", "udw", test_window_stop_1_fourstep);
    add_test("dma stop window2 quick", "udw", test_window_stop_2_quick);
    add_test("dma stop window2 one step", "udw", test_window_stop_2_onestep);
    add_test("dma stop window2 two step", "udw", test_window_stop_2_twostep);
    add_test("dma stop window2 three step", "udw", test_window_stop_2_threestep);
    add_test("dma stop window2 four step", "udw", test_window_stop_2_fourstep);
    add_test("dma stop window3 quick", "udw", test_window_stop_3_quick);
    add_test("dma stop window3 one step", "udw", test_window_stop_3_onestep);
    add_test("dma stop window3 two step", "udw", test_window_stop_3_twostep);
    add_test("dma stop window3 three step", "udw", test_window_stop_3_threestep);
    add_test("dma stop window3 four step", "udw", test_window_stop_3_fourstep);
    add_test("dma stop window4 quick", "udw", test_window_stop_4_quick);
    add_test("dma stop window4 one step", "udw", test_window_stop_4_onestep);
    add_test("dma stop window4 two step", "udw", test_window_stop_4_twostep);
    add_test("dma stop window4 three step", "udw", test_window_stop_4_threestep);
    add_test("dma stop window4 four step", "udw", test_window_stop_4_fourstep);
}
