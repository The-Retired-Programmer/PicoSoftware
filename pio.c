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
// PIO is used to read the logic pins 
//

#include "pio.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

const PIO pio = pio0;
const uint sm = 0;

struct probe_controls* controls;


void pio_init(struct probe_controls* probecontrols) {
    controls = probecontrols;
    uint16_t capture_prog_instr = pio_encode_in(controls->pinbase, controls->pinwidth);
    struct pio_program capture_prog = {
            .instructions = &capture_prog_instr,
            .length = 1,
            .origin = -1
    };
    uint offset = pio_add_program(pio, &capture_prog);
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_in_pins(&c, controls->pinbase);
    sm_config_set_wrap(&c, offset, offset);
    float div = (float) clock_get_hz(clk_sys)/controls->frequency;
    sm_config_set_clkdiv(&c, div);
    sm_config_set_in_shift(&c, true, true, 32);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    pio_sm_init(pio, sm, offset, &c);
}

void pio_arm() {
    pio_sm_set_enabled(pio, sm, false);
    pio_sm_clear_fifos(pio, sm);
    pio_sm_restart(pio, sm);
    pio_sm_exec(pio, sm, pio_encode_wait_gpio(controls->st_trigger==TRIGGER_ON_HIGH, controls->st_pin));
    pio_sm_set_enabled(pio, sm, true);
}

uint get_read_dreq() {
    return pio_get_dreq(pio, sm, false);
}//=============================================================================
//
// the "public" API for the probe
//
//

io_ro_32 *getPIOsource() {
    return &pio->rxf[sm];
}
