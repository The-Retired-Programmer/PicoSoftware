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

//
// TEST CONTROL BLOCK - 1 per test to be run
//

struct test_control_block {
    void (*testfunction)();
    bool success;
};

bool add_test( void (*testfunction)());

void execute_tests();

void summary_of_tests();

void pass_if_null(void* value);

void pass_if_equal_string(char* expected, char* actual);

#endif
