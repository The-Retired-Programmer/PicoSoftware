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
// test - pio_program
//

#include <stdlib.h>
#include "pico/stdlib.h"
#include "ptest.h"
#include "pio_program.h"

static void test_pio_program() {
    struct ppb_config *ppb;
    ppb = ppb_init(pio1,0);
    ppb_set_wraptarget(ppb);
    ppb_add_instruction(ppb, pio_encode_set(pio_pins,7));
    ppb_add_instruction(ppb, pio_encode_set(pio_pins,3));
    ppb_add_instruction(ppb, pio_encode_set(pio_pins,5));
    ppb_add_instruction(ppb, pio_encode_set(pio_pins,1));
    ppb_add_instruction(ppb, pio_encode_set(pio_pins,6));
    ppb_add_instruction(ppb, pio_encode_set(pio_pins,2));
    ppb_add_instruction(ppb, pio_encode_set(pio_pins,4));
    ppb_add_instruction(ppb, pio_encode_set(pio_pins,0));
    ppb_set_wrap(ppb);
    ppb_add_instruction(ppb, pio_encode_jmp_condition(piojmp_Xnot0_minus,ppb_here(ppb))|pio_encode_delay(31));
    //
    ppb_build(ppb);
    pio_sm_config c = ppb_clear_and_load(ppb, 10000.0);
    pass_if_equal_uint("wraptarget",  0 , ppb->wraptarget);
    pass_if_equal_uint( "wrap", 7, ppb->wrap);
    pass_if_equal_uint( "program size", 9, ppb->program.length);
    pass_if_equal_uint( "program offset", 32-9,  ppb->offset);
    //
    uint16_t expected_instructions[] = {0xE007,0xE003,0xE005,0xE001,
        0xE006, 0xE002,0xE004, 0xE000, 0x1F48};
    uint16_t* instructions = ppb->program_instructions;
    for (uint i = 0; i< ppb->program.length; i++) {
       pass_if_equal_uintx("instruction", expected_instructions[i], instructions[i]); 
    }
    c=ppb_load(ppb, 10000.0); // load it again and check it's at a new location
    pass_if_equal_uint( "program offset(2)", 32-9-9, ppb->offset);
    //
    c=ppb_load(ppb, 10000.0); // load it again and check it's at a new location
    pass_if_equal_uint( "program offset(3)", 32-9-9-9, ppb->offset);
    //
    c=ppb_clear_and_load(ppb, 10000.0); // load it again and check it's at the start location
    pass_if_equal_uint( "program offset(4)", 32-9, ppb->offset);
    teardown_ppb(ppb);
}

// =============================================================================
//
// module API
//
// =============================================================================

void test_pio_program_init() {
    add_test("pio_program", "u", test_pio_program);
}
