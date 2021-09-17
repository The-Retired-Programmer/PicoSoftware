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
#include <ctype.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "ptest.h"
#include "../src/logic_probe.h"

// TEST HARNESS

#define END 0
#define QUIT -1
#define OUT_OF_RANGE -2

#define MAXTESTS 100

static struct test_control_block tcbs[MAXTESTS];
static uint testcount = 0;
static struct test_control_block *current_tcb;
static uint current_test_number=0;

static int selected_test_numbers[MAXTESTS+1];
static uint selected_test_insert_point;

static void init_tests_selected() {
    selected_test_insert_point = 0;
}

static void add_test_to_selected(int test_number) {
    if (selected_test_insert_point<MAXTESTS) selected_test_numbers[selected_test_insert_point++]= test_number;
}

static bool is_selected_tests_empty() {
    return selected_test_insert_point == 0;
}

static int *terminate_tests_selected() {
    selected_test_numbers[selected_test_insert_point] = END;
    return selected_test_numbers;
}

static int *insert_test_and_terminate(int test_number) {
    add_test_to_selected(test_number);
    return terminate_tests_selected();
}

static void gettestresponse(char* ptr) {
    putchar('>');
    while (true) {
        char ch = getchar();
        if (ch == '\n' || ch == '\r') {
            *ptr='\0';
            return;
        } else if (ch>= '\x20' && ch <= '\x7e') {
            *ptr++ = ch;
        }
    }
}

static int *parse_alpha(char c) {
    for (int i = 1; i <= testcount;i++){
        struct test_control_block *tcb = &tcbs[i-1];
        if (strchr(tcb->groups,c) != NULL) add_test_to_selected(i);
    }
    return is_selected_tests_empty() ?
                insert_test_and_terminate(OUT_OF_RANGE) :
                terminate_tests_selected();
}

static int *parse_int(char *linebuffer) {
    char *line = linebuffer;
    while(*line != '\0') {
        if (!isdigit(*line++)) {
            return insert_test_and_terminate(OUT_OF_RANGE);
        }
    }
    int val = atoi(linebuffer);
    if (val>0 && val<=testcount) {
        current_test_number = val;
        return insert_test_and_terminate(val);
    }
    return insert_test_and_terminate(OUT_OF_RANGE);
}

static void writemenu() {
    for (int i=0; i < testcount; i++) {
        if (*(tcbs[i].groups) == '\0' ) {
            printf("%i - %s\n", i+1, tcbs[i].name);
        } else 
            printf("%i (%s) - %s\n", i+1, tcbs[i].groups, tcbs[i].name);
    }
}

static void execute_test(int selectionid) {
    current_tcb = &tcbs[selectionid-1];
    current_tcb->passcount = 0;
    current_tcb->failcount = 0;
    printf("RUNNING (%i) %s\n",selectionid, current_tcb->name);
    probe_pass_init();
    trace_init();
    current_tcb->testfunction();
    trace_print();
    probe_pass_teardown();
}

static int *select_all_tests() {
    for (int i = 1; i <= testcount;i++) add_test_to_selected(i);
    return terminate_tests_selected();
}

static void execute_selected_tests() {
    int *selected_tests = selected_test_numbers;
    while (*selected_tests > 0) execute_test(*selected_tests++);
}

static void summary_of_tests() {
    int *selected_tests = selected_test_numbers;
    struct test_control_block *tcb;
    uint tno = 0;
    uint sno = 0;
    uint fno = 0;
    while (*selected_tests > 0) {
        tcb = &tcbs[*selected_tests-1];
        tno++;
        sno+=tcb->passcount;
        fno+=tcb->failcount;
        selected_tests++;
    }
    if (fno == 0) {
        puts("\nTEST STATUS: GREEN");
    } else {
        puts("\nTEST STATUS: RED");
    }
    printf("Tests: %i; Checks: passed: %i, failed: %i;\n", tno, sno, fno);
}

static bool test_if_selection_mode() {
    gpio_init(22);
    gpio_pull_up(22);
    return gpio_get(22)==0;
}

static int *getselectionid() {
    // 1-n => test number ; special codes (quit, out_of_range)  are negatives
    char linebuffer[120];
    gettestresponse(linebuffer);
    char c = linebuffer[0];
    switch (c) {
    case ';': case ':':
        return insert_test_and_terminate(QUIT);
    case '#': case '~':
        return select_all_tests();
    case '?': case '/':
        writemenu();
        return getselectionid();
    case '@': case '\'':
        if (current_test_number == 0 ) {
            return insert_test_and_terminate(++current_test_number);
        }
        return insert_test_and_terminate(current_test_number);
    case '\0': case '>': case '.':
        if (current_test_number < testcount) {
            return insert_test_and_terminate(++current_test_number);
        }
        return insert_test_and_terminate(OUT_OF_RANGE);
    case '<': case ',':
        if (current_test_number > 1) {
            return insert_test_and_terminate(--current_test_number);
        }
        return insert_test_and_terminate(OUT_OF_RANGE);
    case '0' ... '9':
        return parse_int(linebuffer);
    case 'a' ... 'z':
        return parse_alpha(c);
    case 'A' ... 'Z':
        return parse_alpha(tolower(c));
    default:
        return insert_test_and_terminate(OUT_OF_RANGE);
    }
}

//      TRACE

#define TRACEBUFFERSIZE 1000
#define PRINTTRACELINESIZE 100
static char tracebuffer[TRACEBUFFERSIZE];
static uint insertpoint;
static bool rotated;

static void trace_print_segment(uint frompoint, uint size) {
    for (uint i = frompoint; i < frompoint+ size; i++) {
        putchar(tracebuffer[i]);
    }
}

// =============================================================================
//
// module API
//
// =============================================================================

//
//   TEST HARNESS
//

void ptest_init() {
    testcount = 0;
}

bool add_test(char* name, char* groups, void (*testfunction)() ) {
    if (testcount >= MAXTESTS) {
        return false;
    }
    tcbs[testcount].name = name;
    tcbs[testcount].groups = groups;
    tcbs[testcount].passcount = 0;
    tcbs[testcount].failcount = 0;
    tcbs[testcount++].testfunction = testfunction;
    return true;
}

void ptest_execute() {
    puts("\nTESTING STARTING ...\n");
    if (test_if_selection_mode()){
        while (true) {
            init_tests_selected();
            int* selectedtest = getselectionid();
            if (*selectedtest == QUIT) break;
            else if (*selectedtest > 0) {
               execute_selected_tests();
               summary_of_tests();
            }
        }
    } else {
        init_tests_selected();
        select_all_tests();
        execute_selected_tests();
        summary_of_tests();
    }
    puts("\n ...TESTING COMPLETED\n");
}

//
//      TRACE
//

void trace_init() {
    for (uint i = 0; i < TRACEBUFFERSIZE; i++) {
        tracebuffer[i] = ' ';
    }
    insertpoint = 0;
    rotated = false;
}

void trace(char tracechar) {
    if (insertpoint >= TRACEBUFFERSIZE) {
        insertpoint = 0;
        rotated = true;
    }
    tracebuffer[insertpoint++] = tracechar;
}

void trace_print() {
    if (!rotated && insertpoint==0) return;
    uint startpoint = rotated ? insertpoint+1 : 0;
    uint size = rotated ? TRACEBUFFERSIZE : insertpoint;
    puts("Trace:");
    if (rotated) puts(".....");
    while (size > 0) {
        uint printsize = size < PRINTTRACELINESIZE? size: PRINTTRACELINESIZE;
        if (startpoint + printsize > TRACEBUFFERSIZE) {
            uint segsize = TRACEBUFFERSIZE - startpoint;
            trace_print_segment(startpoint, segsize);
            startpoint = 0;
            segsize = printsize - segsize;
            trace_print_segment(startpoint, segsize);
        } else {
            trace_print_segment(startpoint, printsize);
        }
        putchar('\n');
        size -= printsize;
        startpoint += printsize;
    }
}

//
//      PASS / FAIL TESTS
//

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

void pass_if_equal_uintx(char* id, uint expected, uint value) {
    if (expected == value) {
        current_tcb->passcount++;
    } else {
        current_tcb->failcount++;
        printf("Test %s - Check %s failed\n", current_tcb->name, id);
        printf("    expected %x, was %x\n", expected, value);
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

void pass_if_equal_uint32x(char* id, uint32_t expected, uint32_t value) {
    if (expected == value) {
        current_tcb->passcount++;
    } else {
        current_tcb->failcount++;
        printf("Test %s - Check %s failed\n", current_tcb->name, id);
        printf("    expected %x, was %x\n", expected, value);
    }
}

void pass_if_greaterthan_uint(char* id, uint expected, uint value) {
    if (expected < value) {
        current_tcb->passcount++;
    } else {
        current_tcb->failcount++;
        printf("Test %s - Check %s failed\n", current_tcb->name, id);
        printf("    expected %x, was %x\n", expected, value);
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

void pass(char* id) {
    current_tcb->passcount++;
}
