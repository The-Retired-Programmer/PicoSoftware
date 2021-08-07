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

void ptest_init() {
    testcount = 0;
}

bool add_test(char* name, void (*testfunction)() ) {
    if (testcount >= MAXTESTS) {
        return false;
    }
    tcbs[testcount].name = name;
    tcbs[testcount].passcount = 0;
    tcbs[testcount].failcount = 0;
    tcbs[testcount++].testfunction = testfunction;
    return true;
}

void execute_tests() {
    for (uint i = 0; i < testcount; i++) {
        current_tcb = &tcbs[i];
        current_tcb->testfunction();
    }
}

void summary_of_tests() {
    uint tno = 0;
    uint sno = 0;
    uint fno = 0;
    for (uint i = 0; i < testcount; i++) {
        tno++;
        sno+=tcbs[i].passcount;
        fno+=tcbs[i].failcount;
    }
    if (fno == 0) {
        puts("\nTEST STATUS: GREEN");
    } else {
        puts("\nTEST STATUS: RED");
    }
    printf("Tests: %i; Checks: passed: %i, failed: %i;\n", tno, sno, fno);
}

// ----------------------------------------------------------------------------
//
// The various pass tests for use in tests
//
// ----------------------------------------------------------------------------

void pass_if_null(char* id, char* value){
    if (NULL == value) {
        current_tcb->passcount++;
    } else {
        current_tcb->failcount++;
        printf("Test %s - Check %s failed", current_tcb->name, id);
        printf("    expected NULL, was %s\n",   value);
    }
}

void pass_if_equal_string(char* id, char* expected, char* value) {
    if (strcmp(expected,value) == 0) {
        current_tcb->passcount++;
    } else {
        current_tcb->failcount++;
        printf("Test %s - Check %s failed\n", current_tcb->name, id);
        printf("    expected %s, was %s\n", expected, value);
    }
}

void pass_if_equal_uint(char* id, uint expected, uint value) {
    if (expected == value) {
        current_tcb->passcount++;
    } else {
        current_tcb->failcount++;
        printf("Test %s - Check %s failed\n", current_tcb->name, id);
        printf("    expected %i, was %i\n", expected, value);
    }
}

void pass_if_equal_uint32(char* id, uint32_t expected, uint32_t value) {
    if (expected == value) {
        current_tcb->passcount++;
    } else {
        current_tcb->failcount++;
        printf("Test %s - Check %s failed\n", current_tcb->name, id);
        printf("    expected %i, was %i\n", expected, value);
    }
}

void pass_if_true_with_message(char* id, bool value, char* message) {
    if (value) {
        current_tcb->passcount++;
    } else {
        current_tcb->failcount++;
        printf("Test %s - Check %s failed\n", current_tcb->name, id);
        printf("    expected true, was false - %s\n", message);
    }
}

void pass_if_true(char* id, bool value) {
    if (value) {
        current_tcb->passcount++;
    } else {
        current_tcb->failcount++;
        printf("Test %s - Check %s failed\n", current_tcb->name, id);
        puts("    expected true, was false");
    }
}

void pass_if_false_with_message(char* id, bool value, char* message) {
    if (!value) {
        current_tcb->passcount++;
    } else {
        current_tcb->failcount++;
        printf("Test %s - Check %s failed\n", current_tcb->name, id);
        printf("    expected false, was true - %s\n", message);
    }
}

void pass_if_false(char* id, bool value) {
    if (!value) {
        current_tcb->passcount++;
    } else {
        current_tcb->failcount++;
        printf("Test %s - Check %s failed\n", current_tcb->name, id);
        puts("    expected false, was true");
    }
}

void fail(char* id) {
    current_tcb->failcount++;
    printf("Test %s - Check %s failed\n", current_tcb->name, id);
    puts("    fail");
}
