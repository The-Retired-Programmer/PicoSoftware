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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "ptest.h"

#define MAXTESTS 20 

static struct test_control_block tcbs[MAXTESTS];
static uint testcount = 0;
static struct test_control_block *current_tcb;

bool add_test(void (*testfunction)() ) {
    if (testcount >= MAXTESTS) {
        return false;
    }
    tcbs[testcount].success = false;
    tcbs[testcount++].testfunction = testfunction;
    return true;
}

void execute_tests() {
    for (uint i = 0; i < testcount; i++) {
        current_tcb = &tcbs[i];
        current_tcb->testfunction();
    }
}

void pass_if_null(void* value){
    bool success = NULL == value;
    current_tcb->success = success;
    if (!success) {
        printf("Test failed: expected NULL - actual \"%s\"\n", value);
    }
}

void pass_if_equal_string(char* expected, char* actual) {
    bool success = strcmp(expected,actual) == 0;
    current_tcb->success = success;
    if (!success) {
        printf("Test failed: expected \"%s\" - actual \"%s\"\n", expected, actual);
    }
}

void summary_of_tests() {
    uint tno = 0;
    uint sno = 0;
    uint fno = 0;
    for (uint i = 0; i < testcount; i++) {
        bool success = tcbs[i].success;
        tno++;
        if (success) sno++;
        if (!success) fno++;
    }
    if (tno == sno) {
        puts("\nTEST STATUS: GREEN");
    } else {
        puts("\nTEST STATUS: RED");
    }
    printf("Tests: %i; Successful: %i; Failed: %i\n", tno, sno, fno);
}
