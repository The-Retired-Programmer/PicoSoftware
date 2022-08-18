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
#include "ptest.h"
#include "test_probe_controls.h"
#include "test_gpio_probe_event.h"
#include "test_pio_program.h"
#include "test_square_wave_generator.h"
#include "test_run_length_encoder.h"
#include "test_pio_digitalsampling.h"
#include "test_dma_digitalsampling.h"
#include "integration_test.h"

int main() {
    stdio_init_all();
    ptest_init();
    //
    // define all tests
    //
    test_probe_controls_init();
    test_gpio_probe_event_init();
    test_pio_program_init();
    test_square_wave_generator_init();
    test_run_length_encoder_init();
    test_pio_digitalsampling_init();
    test_dma_digitalsampling_init();
    integration_test_init();
    //
    // run the tests
    //
    ptest_execute();
}
