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
#include "ptest.h"
#include "../src/digitalsampling.h"
#include "../src/digitalsampling_internal.h"
#include "../src/pio_program.h"
#include "../src/pio_program_internal.h"
#include "test_digitalsampling.h"

void test_digitalsampling_init() {
    add_test("test_digitalsampling_rle_internals", "rle", test_digitalsampling_rle_internals);
    add_test("test_digitalsampling_dma_internals", "dma", test_digitalsampling_dma_internals);
    //add_test("test_digitalsampling_pio_internals","pio", test_digitalsampling_pio_internals);
}

void test_digitalsampling_rle_internals() {
    // basic alternating levels
    init_rle(1,13,rlelinereceiverunused);
    bool level = true;
    for (uint i = 0; i<10;i++) {
        rle_insertvalue(level);
        level = !level;
    }
    writetobuffer(); // flush
    pass_if_equal_string("basic alternating levels", "HLHLHLHLHL", get_linebuffer());
    // basic alternating levels - split after 9
    init_rle(1,12,rlelinereceiver9chars);
    level = true;
    for (uint i = 0; i<10;i++) {
        rle_insertvalue(level);
        level = !level;
    }
    writetobuffer(); // flush
    pass_if_equal_string("basic alternating levels - split after 9", "L", get_linebuffer());
    // basic alternating levels bilevels
    init_rle(1,30,rlelinereceiverunused);
    level = true;
    for (uint i = 0; i<10;i++) {
        rle_insertvalue(level);
        rle_insertvalue(level);
        level = !level;
    }
    writetobuffer(); // flush
    pass_if_equal_string("basic alternating bilevels", "2H2L2H2L2H2L2H2L2H2L", get_linebuffer());
    // very short line size
    init_rle(1,11,rlelinereceiver);
    level = true;
    for (uint i = 0; i<10;i++) {
        rle_insertvalue(level);
        rle_insertvalue(level);
        level = !level;
    }
    writetobuffer(); // flush
    pass_if_equal_string("final small line", "2H2L", get_linebuffer());
}

int rlelinereceiver(const char *line) {
    pass_if_equal_string("small line receiver", "2H2L2H2L", get_linebuffer());
    return 0;
}

int rlelinereceiverunused(const char *line) {
    fail("unexpected line segment received");
    return 0;
}

int rlelinereceiver9chars(const char *line) {
    pass_if_equal_string("small line receiver - 9 alternating", "HLHLHLHLH", get_linebuffer());
    return 0;
}

const volatile uint32_t readdata = 0xcccccccc;
uint dma_buffer_fills = 0 ;

//
//   you need approx 2micro secs per buffer fill to be able to count the individual
//   irq0 (control dma) done interrupts
//   -   so running at fully bus speed and system clock of 125Mhz and 4 capture buffers,
//       32K samples is sufficient (min for full speed)
//
//   max sample size (1 pin sample) is approx 1,920,000 bits - will obviously reduce as
//   code base increases. ( a factor of 60 over minimum - at full speed).
//
void test_digitalsampling_dma_internals() {
    dma_buffer_fills = 0;
    struct probe_controls controls;
    char* res = setup_controls(&controls,"g-16-1-19200-1-16-0-0-16-0-1-960000");
    if ( res != NULL ) {
        fail(res);
        return;
    }
    if (!( 
            setuptransferbuffers(&controls)
            && setupDMAcontrollers(&controls, &readdata, 0x3f, dma_buffer_callback, dma_transfer_finished_callback)
        )) {
        fail(geterrormessage());
        return;
    };
    dma_start();
    while (!is_completed());
    pass("transfer completed signalled");
    uint32_t **b = getcapturebuffers();
    for (uint i = 0; i < 4; i++) {
        uint32_t *bp = *(b++);
        pass_if_equal_uint32("buffer check", readdata, *bp);
    };
    pass_if_equal_uint("control count", 5, dma_buffer_fills);

}

char* setup_controls(struct probe_controls* controls, char * cmd) {
    char cmdbuffer[255]; 
    return parse_control_parameters(controls, strcpy(cmdbuffer,cmd));
}

void dma_buffer_callback() {
    dma_buffer_fills++;
}

void dma_transfer_finished_callback() {
    // null for moment
}

// needs a link from pins 15 to 16 to connect squarewave generator output pin
// to probe pin
void test_digitalsampling_pio_internals() {
    dma_buffer_fills = 0;
    struct probe_controls controls;
    char* res = setup_controls(&controls,"g-16-1-3200-1-16-0-0-16-0-1-3200");
    if ( res != NULL ) {
        fail(res);
        return;
    }
    if (!( 
            setupPIOandSM(&controls)
        )) {
        fail(geterrormessage());
        return;
    };
    waitforstartevent(&controls);
    //squarewavegenerator_init(pio1, 0, 13, 100);
    pio_start();
    //read from pio fifo
    #define READSIZE 10
    uint32_t databuffer[READSIZE];
    for (uint i = 0; i < READSIZE; i++ ) {
        databuffer[i] = pioread();
        printf("%8X\n",databuffer[i]);
    }
}
