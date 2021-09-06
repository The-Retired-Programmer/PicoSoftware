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
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "ptest.h"
#include "../src/square_wave_generator.h"
#include "../src/digitalsampling.h"
#include "../src/run_length_encoder.h"
#include "../src/pio_program.h"
#include "../src/pio_digitalsampling.h"
#include "../src/dma_digitalsampling.h"
#include "test_digitalsampling.h"

static int rlelinereceiver(const char *line) {
    pass_if_equal_string("small line receiver", "2H2L2H2L", get_rle_linebuffer());
    return 0;
}

static int rlelinereceiverunused(const char *line) {
    fail("unexpected line segment received");
    return 0;
}

static int rlelinereceiver9chars(const char *line) {
    pass_if_equal_string("small line receiver - 9 alternating", "HLHLHLHLH", get_rle_linebuffer());
    return 0;
}

static void test_digitalsampling_rle_internals() {
    // basic alternating levels
    rle_init(1,13,rlelinereceiverunused);
    bool level = true;
    for (uint i = 0; i<10;i++) {
        rle_insertvalue(level);
        level = !level;
    }
    rle_writetobuffer(); // flush
    pass_if_equal_string("basic alternating levels", "HLHLHLHLHL", get_rle_linebuffer());
    // basic alternating levels - split after 9
    rle_init(1,12,rlelinereceiver9chars);
    level = true;
    for (uint i = 0; i<10;i++) {
        rle_insertvalue(level);
        level = !level;
    }
    rle_writetobuffer(); // flush
    pass_if_equal_string("basic alternating levels - split after 9", "L", get_rle_linebuffer());
    // basic alternating levels bilevels
    rle_init(1,30,rlelinereceiverunused);
    level = true;
    for (uint i = 0; i<10;i++) {
        rle_insertvalue(level);
        rle_insertvalue(level);
        level = !level;
    }
    rle_writetobuffer(); // flush
    pass_if_equal_string("basic alternating bilevels", "2H2L2H2L2H2L2H2L2H2L", get_rle_linebuffer());
    // very short line size
    rle_init(1,11,rlelinereceiver);
    level = true;
    for (uint i = 0; i<10;i++) {
        rle_insertvalue(level);
        rle_insertvalue(level);
        level = !level;
    }
    rle_writetobuffer(); // flush
    pass_if_equal_string("final small line", "2H2L", get_rle_linebuffer());
}

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
    // don't award dma bus priority else it grabs all cycles (even from irq callbacks)
    //dma_to_have_bus_priority();
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
    // don't award dma bus priority else it grabs all cycles (even from irq callbacks)
    //dma_to_have_bus_priority();
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

static void test_digitalsampling_pio_internals() {
    struct probe_controls controls;
    char* res = parse_control_parameters(&controls,"g-13-3-20000-1-13-3-0-13-0-1-3200");
    if ( res != NULL ) {
        fail(res);
        return;
    }
    piodigitalsampling_init(&controls);
    piodigitalsampling_start();
    square_wave_generator(13, 3, 1250);
    //read from pio fifo
    #define READSIZE 100
    uint32_t databuffer[READSIZE];
    for (uint i = 0; i < READSIZE; i++ ) {
        databuffer[i] = piodigitalsampling_read();
    }
    pass_if_equal_uintx("buffer read", 0x3B7693B2, databuffer[0]);
}

// =============================================================================
//
// module API
//
// =============================================================================

void test_digitalsampling_init() {
    add_test("run length encoder", "rle", test_digitalsampling_rle_internals);
    add_test("dma digitalsampling", "dma", test_digitalsampling_dma_internals);
    add_test("dma stop", "dma_stop", test_digitalsampling_dma_stop);
    add_test("pio digitalsampling","pio", test_digitalsampling_pio_internals);
}
