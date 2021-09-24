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
// test - square wave generator
//

#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "../src/square_wave_generator.h"
#include "ptest.h"

// no pass or fail - it needs a visual check on GPIO pins with a scope or leds

static void test_swg(uint32_t usecs) {
    square_wave_generator_init(19,0.25);
    square_wave_generator_start();
    busy_wait_us_32(usecs);
    teardown_square_wave_generator();
}

static void test_square_wave_generator_odd() {
    test_swg(9000000);
}

static void test_square_wave_generator_even() {
    test_swg(6000000);
}

static void test_square_wave_generator_half() {
    test_swg(4500000);
}

// =============================================================================
//
// module API
//
// =============================================================================

void test_square_wave_generator_init() {
    add_test("square_wave_generator - 6 secs", "q", test_square_wave_generator_even);
    add_test("square_wave_generator - 9 secs", "q", test_square_wave_generator_odd);
    add_test("square_wave_generator - 4.5 secs", "q", test_square_wave_generator_half);
}
