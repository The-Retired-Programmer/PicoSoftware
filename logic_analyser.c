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
// LOGIC ANALYSER - using Pico as Probe
//

#include "pio.h"
#include "storage.h"
#include "sourcesignal.h"

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"

const uint PIN_BASE = 16;
const uint PIN_COUNT = 2;
const uint SAMPLES = 96;

void print_capture_buf(uint32_t *buf, uint pin_base, uint pin_count, uint32_t n_samples);

int main() {
    stdio_init_all();
    printf("Initialising ...\n");
    storage_init(SAMPLES, PIN_COUNT);
    pio_init(PIN_BASE, PIN_COUNT, 1.f);
    printf("Arming ...\n");
    pio_arm(PIN_BASE, true);
    storage_arm();
    printf("Capturing ...\n");
    source_run(PIN_BASE);
    storage_waituntilcompleted();
    printf("Completed\n");
    print_capture_buf(get_capturebuf(), PIN_BASE, PIN_COUNT, SAMPLES);
}

void print_capture_buf(uint32_t *buf, uint pin_base, uint pin_count, uint32_t n_samples) {
    printf("Output:\n");
    for (int pin = 0; pin < pin_count; ++pin) {
        printf("%02d: ", pin + pin_base);
        for (int sample = 0; sample < n_samples; ++sample) {
            uint bit_index = pin + sample * pin_count;
            uint word_index = bit_index / 32;
            uint word_mask = 1u << (bit_index % 32);
            printf(buf[word_index] & word_mask ? "-" : "_");
        }
        printf("\n");
    }
}
