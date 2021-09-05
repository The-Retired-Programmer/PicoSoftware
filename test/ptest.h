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

#ifndef _PTEST_H
#define _PTEST_H

#include <stdlib.h>
#include "pico/stdlib.h"

//      TEST HARNESS

struct test_control_block {
    char* name;
    char* alias; // short for single test UI use
    void (*testfunction)();
    int passcount;
    int failcount;
};

void ptest_init();

bool add_test(char* name, char* alias, void (*testfunction)());

void ptest_execute();

//      TRACE

void trace_init();

void trace(char tracechar);

void trace_print();

//      PASS / FAIL TESTS

void pass_if_null(char* id, char* value);

void pass_if_equal_string(char* id, char* expected, char* value);
void pass_if_equal_uint(char* id, uint expected, uint value);
void pass_if_equal_uintx(char* id, uint expected, uint value);
void pass_if_equal_uint32(char* id, uint32_t expected, uint32_t value);
void pass_if_equal_uint32x(char* id, uint32_t expected, uint32_t value);

void pass_if_greaterthan_uint(char* id, uint expected, uint value);

void pass_if_true_with_message(char* id, bool value, char* message);
void pass_if_true(char* id, bool value);

void pass_if_false_with_message(char* id, bool value, char* message);
void pass_if_false(char* id, bool value);

void fail(char* id);
void pass(char* id);

#endif
