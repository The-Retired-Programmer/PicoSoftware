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
#include "ptest.h"
#include "../src/square_wave_generator.h"
#include "../src/pio_digitalsampling.h"

static void test_digitalsampling_pio_internals() {
    struct probe_controls controls;
    char* errormessage = parse_control_parameters(&controls,"g-19-3-20000-1-19-3-0-19-0-1-3200");
    if ( errormessage != NULL ) {
        fail(errormessage);
        return;
    }
    piodigitalsampling_init(&controls);
    piodigitalsampling_start();
    //
    errormessage = square_wave_generator_init(19,1250);
    if ( errormessage != NULL ) {
        fail(errormessage);
        return;
    }
    square_wave_generator_start();
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

void test_pio_digitalsampling_init() {
    add_test("pio digitalsampling","u", test_digitalsampling_pio_internals);
}
