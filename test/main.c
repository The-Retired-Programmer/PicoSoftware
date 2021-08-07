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
// Logic Analyser Probe - testing entry point
//

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "ptest.h"
#include "test_probe_controls.h"
#include "test_digitalsampling.h"

int main() {
    stdio_init_all();
    ptest_init();
    // define all tests
    test_probe_controls_init();
    test_digitalsampling_init();
    // run the tests
    puts("\nTESTING STARTING ...\n");
    execute_tests();
    summary_of_tests();
    puts("\n ...TESTING COMPLETED\n");
}
