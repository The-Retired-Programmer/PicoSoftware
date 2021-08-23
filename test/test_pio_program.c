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
#include "../src/pio_program.h"
#include "../src/pio_program_internal.h"

void test_pio_program() {
    ppb_init_pio1();
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
    ppb_add_instruction(pio_encode_jmp_condition(piojmp_Xnot0_minus,ppb_here())|pio_encode_delay(31));
    //
    ppb_build();
    pio_sm_config c = ppb_clear_and_load();
    pass_if_equal_uint("wraptarget",  0 , ppb_get_wraptarget());
    pass_if_equal_uint( "wrap", 7, ppb_get_wrap());
    pass_if_equal_uint( "program size", 9, ppb_getprogramsize());
    pass_if_equal_uint( "program offset", 32-9,  ppb_getprogramoffset());
    //
    uint16_t expected_instructions[] = {0xE007,0xE003,0xE005,0xE001,
        0xE006, 0xE002,0xE004, 0xE000, 0x1F48};
    uint16_t* instructions = ppb_getprograminstructions();
    for (uint i = 0; i< ppb_getprogramsize(); i++) {
       pass_if_equal_uintx("instruction", expected_instructions[i], instructions[i]); 
    }
    c=ppb_load(); // load it again and check it's at a new location
    pass_if_equal_uint( "program offset(2)", 32-9-9, ppb_getprogramoffset());
    //
    c=ppb_load(); // load it again and check it's at a new location
    pass_if_equal_uint( "program offset(3)", 32-9-9-9, ppb_getprogramoffset());
    //
    c=ppb_clear_and_load(); // load it again and check it's at the start location
    pass_if_equal_uint( "program offset(4)", 32-9, ppb_getprogramoffset());
}

void test_pio_program_init() {
    add_test("test_pio_program", "pioprog", test_pio_program);
}

