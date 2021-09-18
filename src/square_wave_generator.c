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

// =========================================================================
//
//  SQUARE WAVE GENERATOR
//
//   creates a square wave form at requested frequency
//   and also x2 and x4 versions
//
//   as the 16 bit divisor for frequency speed is not enough to cover a range
//   as high as clock frequency/8 down to 0.25s, there are two PIO programs to
//   cover the full range.  There is an overlap in the ranges, so the 1KHz point
//   has been selected as the change over point.
//
//   high frequency program
//   cycles per wrap = 8; frequency range: <= 15.625MHz  >238.419Hz
//
//   low frequency program
//   cycles per wrap = 8704; frequency range: <= 14.361KHz  >0.219Hz
//
//  USES - PIO1 & SM0, clears all programs from PIO 1 prior to loading its own.
//
// =========================================================================


#include <stdlib.h>
#include "pico/stdlib.h"
#include "pio_program.h"

// =============================================================================
//
// module API
//
// =============================================================================

char *square_wave_generator_init(uint pinbase, float frequency) {
    uint wrap_program_cycles;
    ppb_init_pio1();
    if (frequency > 1000) {
        ppb_set_wraptarget();
        ppb_add_instruction(pio_encode_set(pio_pins,7));
        ppb_add_instruction(pio_encode_set(pio_pins,3));
        ppb_add_instruction(pio_encode_set(pio_pins,5));
        ppb_add_instruction(pio_encode_set(pio_pins,1));
        ppb_add_instruction(pio_encode_set(pio_pins,6));
        ppb_add_instruction(pio_encode_set(pio_pins,2));
        ppb_add_instruction(pio_encode_set(pio_pins,4));
        ppb_add_instruction(pio_encode_set(pio_pins,0));
        ppb_set_wrap();
        wrap_program_cycles = 8;
    } else {
        ppb_set_wraptarget();
        ppb_add_instruction(pio_encode_set(pio_pins,7)|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_set(pio_x, 31)|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_jmp_condition(piojmp_Xnot0_minus,ppb_here())|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_set(pio_pins,3)|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_set(pio_x, 31)|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_jmp_condition(piojmp_Xnot0_minus,ppb_here())|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_set(pio_pins,5)|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_set(pio_x, 31)|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_jmp_condition(piojmp_Xnot0_minus,ppb_here())|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_set(pio_pins,1)|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_set(pio_x, 31)|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_jmp_condition(piojmp_Xnot0_minus,ppb_here())|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_set(pio_pins,6)|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_set(pio_x, 31)|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_jmp_condition(piojmp_Xnot0_minus,ppb_here())|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_set(pio_pins,2)|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_set(pio_x, 31)|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_jmp_condition(piojmp_Xnot0_minus,ppb_here())|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_set(pio_pins,4)|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_set(pio_x, 31)|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_jmp_condition(piojmp_Xnot0_minus,ppb_here())|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_set(pio_pins,0)|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_set(pio_x, 31)|pio_encode_delay(31));
        ppb_add_instruction(pio_encode_jmp_condition(piojmp_Xnot0_minus,ppb_here())|pio_encode_delay(31));
        ppb_set_wrap();
        wrap_program_cycles = 8704;
    }
    char *errormessage = ppb_build();
    if (errormessage != NULL) return errormessage;
    pio_sm_config c = ppb_clear_and_load(wrap_program_cycles*frequency);
    sm_config_set_set_pins(&c, pinbase, 3);
    pio_gpio_init(pio1, pinbase);
    pio_gpio_init(pio1, pinbase+1);
    pio_gpio_init(pio1, pinbase+2);
    pio_sm_set_consecutive_pindirs(pio1, 0, pinbase, 3, true);
    ppb_configure(&c);
    return NULL;
}

void square_wave_generator_start() {
    ppb_start();
}
