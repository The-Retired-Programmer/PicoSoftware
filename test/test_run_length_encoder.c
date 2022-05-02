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
#include "run_length_encoder.h"

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

// =============================================================================
//
// module API
//
// =============================================================================

void test_run_length_encoder_init() {
    add_test("run length encoder", "u", test_digitalsampling_rle_internals);
}
