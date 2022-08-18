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
//  LOGIC PROBE - Digital Sampling using PIO
//
//   Collects sample data from pins
//
//   as the 16 bit divisor for frequency speed is not enough to cover a range
//   as high as clock frequency down to 1s, there are two PIO programs to
//   cover the full range.  There is an overlap in the ranges, so the 10KHz point
//   has been selected as the change over point.
//
//   high frequency program
//   cycles per wrap = 1; frequency range: <= 125MHz  >1.907KHz
//
//   low frequency program 
//   cycles per wrap = 2200; frequency range: <= 56.818KHz  >0.867Hz 
//
// =========================================================================

#include <stdlib.h>
#include "hardware/pio.h"
#include "pio_program.h"
#include "probe_controls.h"

static struct ppb_config *ppb;

// =============================================================================
//
// module API
//
// =============================================================================

void piodigitalsampling_init(struct probe_controls* controls) {
    uint pin = controls->pinbase;
    uint pinwidth = controls->pinwidth;
    float frequency = controls->frequency;
    uint wrap_program_cycles;
    ppb = ppb_init(pio0,0);
    if (controls->st_enabled) {
        switch (controls->st_trigger) {
        case TRIGGER_ON_LOW:
            ppb_add_instruction(ppb, pio_encode_wait_gpio(false,controls->st_pin));
            break;
        case TRIGGER_ON_HIGH:
            ppb_add_instruction(ppb, pio_encode_wait_gpio(true,controls->st_pin));
            break;
        case TRIGGER_ON_FALL:
            ppb_add_instruction(ppb, pio_encode_wait_gpio(true,controls->st_pin));
            ppb_add_instruction(ppb, pio_encode_wait_gpio(false,controls->st_pin));
            break;
        case TRIGGER_ON_RISE:
            ppb_add_instruction(ppb, pio_encode_wait_gpio(false,controls->st_pin));
            ppb_add_instruction(ppb, pio_encode_wait_gpio(true,controls->st_pin));
            break;
        }
    }
    ppb_set_wraptarget(ppb);
    if (frequency > 10000) {
        ppb_add_instruction(ppb, pio_encode_in(pio_pins,pinwidth));
        wrap_program_cycles = 1;
    } else {
        ppb_add_instruction(ppb, pio_encode_in(pio_pins,pinwidth));
        ppb_add_instruction(ppb, pio_encode_set(pio_y,1)|pio_encode_delay(22));
        uint ylabel = ppb_here(ppb);
        ppb_add_instruction(ppb, pio_encode_set(pio_x, 31)|pio_encode_delay(31));
        ppb_add_instruction(ppb, pio_encode_jmp_condition(piojmp_Xnot0_minus,ppb_here(ppb))|pio_encode_delay(31));
        ppb_add_instruction(ppb, pio_encode_jmp_condition(piojmp_Ynot0_minus,ylabel)|pio_encode_delay(31));
        wrap_program_cycles = 2200;
    }
    ppb_set_wrap(ppb);
    ppb_build(ppb);
    pio_sm_config c = ppb_clear_and_load(ppb, wrap_program_cycles*frequency);
    sm_config_set_in_pins(&c, pin);
    sm_config_set_in_shift(&c, false, true, usedbitsperword(controls));
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    ppb_configure(ppb, &c);
}

void teardown_piodigitalsampling() {
    teardown_ppb(ppb);
}

void piodigitalsampling_start() {
    ppb_start(ppb);
}

#ifdef TESTINGBUILD

uint32_t piodigitalsampling_read() {
    return  pio_sm_get_blocking(ppb->pio, ppb->sm);
}

#endif
