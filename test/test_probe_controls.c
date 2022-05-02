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
#include "ptest.h"
#include "probe_controls.h"

static void test_probe_controls() {
    char cmdbuffer[255]; 
    char* cmd ="g-16-1-19200-1-16-0-0-16-0-1-320";
    struct probe_controls controls;
    char* result = parse_control_parameters(&controls, strcpy(cmdbuffer,cmd));
    pass_if_null("return", result);
    pass_if_equal_uint("pinbase", 16, controls.pinbase);
    pass_if_equal_uint("pinwidth", 1, controls.pinwidth);
    pass_if_equal_uint32("frequency", 19200, controls.frequency);
    pass_if_true("st_enabled", controls.st_enabled);
    pass_if_equal_uint("st_pin", 16, controls.st_pin);
    pass_if_equal_uint("st_trigger", TRIGGER_ON_LOW, controls.st_trigger);
    pass_if_false("ev_enabled", controls.ev_enabled);
    pass_if_equal_uint("ev_pin", 16, controls.ev_pin);
    pass_if_equal_uint("ev_trigger", TRIGGER_ON_LOW, controls.ev_trigger);
    pass_if_equal_uint("sampleendmode", BUFFER_FULL, controls.sampleendmode);
    pass_if_equal_uint32("samplesize", 320, controls.samplesize);
}

static void test_probe_pinbase() {
    char* cmd ="16";
    struct probe_controls controls;
    bool res = parse_pinbase(&controls, cmd);
    pass_if_true_with_message("return", res, get_errormessage());
    pass_if_equal_uint("pinbase", 16, controls.pinbase);
}

// =============================================================================
//
// module API
//
// =============================================================================

void test_probe_controls_init() {
    add_test("parse_control_parameters", "u", test_probe_controls);
    add_test("parse_pinbase", "u", test_probe_pinbase);
}
