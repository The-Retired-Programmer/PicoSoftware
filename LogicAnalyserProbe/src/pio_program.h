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

#ifndef _PIO_PROGRAM_H
#define _PIO_PROGRAM_H

#include <stdlib.h>
#include "hardware/pio.h"

#define MAX_PROGRAM_SIZE 32

struct ppb_config {
    PIO pio;
    uint sm;
    uint offset;
    uint16_t program_instructions[MAX_PROGRAM_SIZE];
    uint16_t *nextinstructioninsertpoint;
    int emptyinstructions;
    int wraptarget;
    int wrap;
    struct pio_program program;
};

struct ppb_config *ppb_init(PIO pioval, uint smnum);

void teardown_ppb(struct ppb_config *config);

void ppb_set_wraptarget(struct ppb_config *config);

void ppb_set_wrap(struct ppb_config *config);

uint ppb_here(struct ppb_config *config);

void ppb_add_instruction(struct ppb_config *config, uint16_t instruction);

char *ppb_build(struct ppb_config *config);

pio_sm_config ppb_load(struct ppb_config *config, float piofrequency);

pio_sm_config ppb_clear_and_load(struct ppb_config *config, float piofrequency);

void ppb_configure(struct ppb_config *config, pio_sm_config *c);

void ppb_start(struct ppb_config *config);

enum pico_jmp_condition {
    piojmp_always = 0u,
    piojmp_X_0 = 1u,
    piojmp_Xnot0_minus = 2u,
    piojmp_Y_0 = 3u,
    piojmp_Ynot0_minus = 4u,
    piojmp_XnotY = 5u,
    piojmp_pin = 6u,
    piojmp_OSREnotempty = 7u
};

inline static uint pio_encode_jmp_condition(enum pico_jmp_condition jmp_condition, uint addr) {
    return _pio_encode_instr_and_args(pio_instr_bits_jmp, jmp_condition, addr);
}

#endif