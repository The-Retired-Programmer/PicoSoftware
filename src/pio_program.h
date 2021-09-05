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

void ppb_init_pio0();
void ppb_init_pio1();
void ppb_init_pio0_sm(uint smnum);
void ppb_init_pio1_sm(uint smnum);

void ppb_set_wraptarget();

void ppb_set_wrap();

uint ppb_here();

void ppb_add_instruction(uint16_t instruction);

char *ppb_build();

pio_sm_config ppb_load(float piofrequency);

pio_sm_config ppb_clear_and_load(float piofrequency);

void ppb_configure(pio_sm_config *c);

void ppb_start();

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

#ifdef TESTINGBUILD

int ppb_get_wraptarget();

int ppb_get_wrap();

uint16_t *ppb_getprograminstructions();

uint ppb_getprogramsize();

uint ppb_getprogramoffset();

#endif

#endif