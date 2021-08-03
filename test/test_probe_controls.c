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
#include "../src/probe_controls.h"

void test_probe_controls() {
    char* cmd ="g-16-1-19200-1-16-0-0-16-0-1-320";
    struct probe_controls controls;
    char* result = parse_control_parameters(&controls, cmd);
    pass_if_null(result);
}

void test_probe_controls_init() {
    add_test(test_probe_controls);
}
