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
// PIO Programming - dynamic programs built
//

#include <stdlib.h>
#include <stdio.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pio_program.h"
#include "pio_program_internal.h"

#define MAX_PROGRAM_SIZE 32
static uint16_t program_instructions[MAX_PROGRAM_SIZE];
static uint16_t *nextinstructioninsertpoint = program_instructions;
static int emptyinstructions = MAX_PROGRAM_SIZE;
static int wraptarget = -1;
static int wrap = -1;
//
static struct pio_program program = {
    .instructions = program_instructions,
    .length = 0,
    .origin = -1
};
static uint offset;
static PIO pio;
static uint sm;

void ppb_init_pio0_sm(uint smnum) {
    pio = pio0;
    ppb_init(smnum);
}

void ppb_init_pio1_sm(uint smnum) {
    pio = pio1;
    ppb_init(smnum);
}

void ppb_init_pio0() {
    pio = pio0;
    ppb_init(0);
}

void ppb_init_pio1() {
    pio = pio1;
    ppb_init(0);
}

static void ppb_init(uint smnum) {
    sm = smnum;
    nextinstructioninsertpoint = program_instructions;
    emptyinstructions = MAX_PROGRAM_SIZE;
    wraptarget = -1;
    wrap = -1;
    program.instructions = program_instructions;
    program.length = 0;
    program.origin = -1;
}

uint ppb_here() {
    return MAX_PROGRAM_SIZE - emptyinstructions;
}

void ppb_set_wraptarget() {
    wraptarget = ppb_here();
}

void ppb_set_wrap() {
    wrap = ppb_here() -1;
}

void ppb_add_instruction(uint16_t instruction) {
    if ( --emptyinstructions < 0 ) {
        return;
    }
    *nextinstructioninsertpoint++=instruction;
    program.length++;
}

char *ppb_build() {
    if (emptyinstructions < 0) {
        return "too many instructions defined";
    }
    if (wraptarget == -1) {
        wraptarget = 0;
    }
    if (wrap == -1) {
        wrap = program.length - 1;
    }
    return NULL;
}

pio_sm_config ppb_load(float piofrequency) {
    offset = pio_add_program(pio, &program);
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + wraptarget, offset + wrap);
    float sysfreq= (float) clock_get_hz(clk_sys);
    sm_config_set_clkdiv(&c, sysfreq/piofrequency);
    return c;
}

pio_sm_config ppb_clear_and_load(float piofrequency) {
    pio_clear_instruction_memory(pio);
    return ppb_load(piofrequency);
}

void ppb_configure(pio_sm_config *c) {
    pio_sm_init(pio, sm, offset, c);
    pio_sm_set_enabled(pio, sm, false);
    pio_sm_clear_fifos(pio, sm);
    pio_sm_restart(pio, sm); // to clear OSR/ISR
}

void ppb_start() {
    pio_sm_set_enabled(pio, sm, true);
}

// and for testing only

int ppb_get_wraptarget() {
    return wraptarget;
}

int ppb_get_wrap() {
    return wrap;
}

uint16_t *ppb_getprograminstructions() {
    return program_instructions;
}

uint ppb_getprogramsize() {
    return program.length;
}

uint ppb_getprogramoffset() {
    return offset;
}
