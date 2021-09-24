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
// PIO Programming - dynamic programs builder
//

#include <stdlib.h>
#include <stdio.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pio_program.h"

// =============================================================================
//
// module API
//
// =============================================================================

struct ppb_config *ppb_init(PIO pio, uint sm) {
    struct ppb_config *config= malloc( sizeof (struct ppb_config)); 
    config->sm = sm;
    config->pio = pio;
    config->nextinstructioninsertpoint = config->program_instructions;
    config->emptyinstructions = MAX_PROGRAM_SIZE;
    config->wraptarget = -1;
    config->wrap = -1;
    config->program.instructions = config->program_instructions;
    config->program.length = 0;
    config->program.origin = -1;
    return config;
}

void teardown_ppb(struct ppb_config *config) {
    pio_sm_set_enabled(config->pio, config->sm, false);
    free(config);
}

uint ppb_here(struct ppb_config *config) {
    return MAX_PROGRAM_SIZE - config->emptyinstructions;
}

void ppb_set_wraptarget(struct ppb_config *config) {
    config->wraptarget = ppb_here(config);
}

void ppb_set_wrap(struct ppb_config *config) {
    config->wrap = ppb_here(config) -1;
}

void ppb_add_instruction(struct ppb_config *config, uint16_t instruction) {
    if ( --(config->emptyinstructions) < 0 ) {
        return;
    }
    *(config->nextinstructioninsertpoint++)=instruction;
    config->program.length++;
}

char *ppb_build(struct ppb_config *config) {
    if (config->emptyinstructions < 0) {
        return "too many instructions defined";
    }
    if (config->wraptarget == -1) {
        config->wraptarget = 0;
    }
    if (config->wrap == -1) {
        config->wrap = config->program.length - 1;
    }
    return NULL;
}

pio_sm_config ppb_load(struct ppb_config *config, float piofrequency) {
    config->offset = pio_add_program(config->pio, &(config->program));
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, config->offset + config->wraptarget, config->offset + config->wrap);
    float sysfreq= (float) clock_get_hz(clk_sys);
    sm_config_set_clkdiv(&c, sysfreq/piofrequency);
    return c;
}

pio_sm_config ppb_clear_and_load(struct ppb_config *config, float piofrequency) {
    pio_clear_instruction_memory(config->pio);
    return ppb_load(config, piofrequency);
}

void ppb_configure(struct ppb_config *config, pio_sm_config *c) {
    pio_sm_init(config->pio, config->sm, config->offset, c);
    pio_sm_set_enabled(config->pio, config->sm, false);
    pio_sm_clear_fifos(config->pio, config->sm);
    pio_sm_restart(config->pio, config->sm); // to clear OSR/ISR
}

void ppb_start(struct ppb_config *config) {
    pio_sm_set_enabled(config->pio, config->sm, true);
}
