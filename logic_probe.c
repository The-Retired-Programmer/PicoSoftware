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
// LOGIC ANALYSER PROBE - using Pico as Probe
//

#include "pio.h"
#include "storage.h"
#include "sourcesignal.h"
#include "logic_probe.h"
#include "logic_analyser.h"

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"

const uint PIN_BASE = 16;
//const int PIN_WIDTH = TWO;
const int PIN_WIDTH = ONE;
const uint SAMPLES = 96;


// configuration data
uint32_t frequency;

void print_capture_buf(uint pin_base, int pin_width, uint32_t n_samples) {
    printf("%s\n", get_RLE_encoded_sample(pin_base));
}

void probe_set_HZsamplerate(double rate) {
    frequency = rate;
}

void probe_set_KHZsamplerate(double rate) {
    frequency = rate*1000;
}
void probe_set_MHZsamplerate(double rate) {
    frequency = rate*1000000;
}

void probe_init() {
    frequency = clock_get_hz(clk_sys);
}

void probe_go() {
    info_response("Initialising ...");
    storage_init(SAMPLES, PIN_BASE, PIN_WIDTH);
    pio_init(PIN_BASE, PIN_WIDTH, 9600*2);
    info_response("Arming ...");
    //pio_arm(PIN_BASE, true);
    pio_arm(PIN_BASE, false);
    storage_arm();
    info_response("Capturing ...");
    storage_waituntilcompleted();
    info_response("Completed");
    print_capture_buf(PIN_BASE, PIN_WIDTH, SAMPLES);
}

uint32_t *get_buf_wordptr(uint word_index) {
    uint pageindex = word_index/get_capturebuf_size();
    //hard_assert(pageindex < get_bufs_count());
    uint pageoffset = word_index%get_capturebuf_size();
    return get_capturebuf(pageindex); 

}
