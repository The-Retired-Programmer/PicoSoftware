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
#include "pico/stdlib.h"
#include "ptest.h"
#include "../src/digitalsampling.h"
#include "../src/digitalsampling_internal.h"
#include "test_digitalsampling.h"

void test_digitalsampling_init() {
    add_test("test_digitalsampling_rle_internals", test_digitalsampling_rle_internals);
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

void rlelinereceiver(const char *line) {
    pass_if_equal_string("small line receiver", "2H2L2H2L", get_linebuffer());
}

void rlelinereceiverunused(const char *line) {
    fail("unexpected line segment received");
}

void rlelinereceiver9chars(const char *line) {
    pass_if_equal_string("small line receiver - 9 alternating", "HLHLHLHLH", get_linebuffer());
}

