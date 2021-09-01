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
// Logic Analyser Probe - testing 
//

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "ptest.h"
#include "test_probe_controls.h"
#include "test_digitalsampling.h"
#include "test_square_wave_generator.h"
#include "test_pio_program.h"
#include "test_gpio_probe_event.h"
#include "integration_test.h"

int main() {
    stdio_init_all();
    ptest_init();
    //
    // define all tests
    //
    test_pio_program_init();
    test_square_wave_generator_init();
    test_probe_controls_init();
    test_digitalsampling_init();
    test_gpio_probe_event_init();
    integration_test_init();
    //
    // run the tests
    //
    puts("\nTESTING STARTING ...\n");
    if (test_if_selection_mode()){
        while (true) {
            int selectedtest = getselectionid();
            if (selectedtest == 0) break;
            if (selectedtest == out_of_range) {
                continue;
            } else if (selectedtest > 0) {
                execute_test(selectedtest);
                summary_of_test();
            } else {
                execute_tests();
                summary_of_tests();
            }
        }
    } else {
        execute_tests();
        summary_of_tests();
    }
    puts("\n ...TESTING COMPLETED\n");
}
