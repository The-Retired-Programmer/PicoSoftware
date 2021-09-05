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

#define MAXTESTS 50 

static struct test_control_block tcbs[MAXTESTS];
static uint testcount = 0;
static struct test_control_block *current_tcb;
static uint singletestnumber=1;

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

static bool isint(char *line) {
    while(*line != '\0') {
        if (!isdigit(*line++)) {
            return false;
        }
    }
    return true;
}

static void writemenu() {
    for (int i=0; i < testcount; i++) {
        printf("%s(%i) - %s\n", tcbs[i].alias, i+1, tcbs[i].name);
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

static void summary_of_test() {
    if (tcbs[singletestnumber-1].failcount == 0) {
        puts("\nTEST STATUS: GREEN");
    } else {
        puts("\nTEST STATUS: RED");
    }
    printf("Checks: passed: %i, failed: %i;\n",
        tcbs[singletestnumber-1].passcount, tcbs[singletestnumber-1].failcount);
}

static void execute_tests() {
    for (uint i = 1; i <= testcount; i++) {
       execute_test(i);
    }
}

static void summary_of_tests() {
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

static bool test_if_selection_mode() {
    gpio_init(22);
    gpio_pull_up(22);
    return gpio_get(22)==0;
}

#define OUT_OF_RANGE -2
#define ALL -1
#define QUIT 0

static int getselectionid() {
    // 1-n => test number ; 0 is  exit ; -1 is all ; -2 is out of range
    // user command format: decimal digits (nn= test number or 0 = quit)
    //   + = next test number ; empty (ie just \n ) is repeat; - = previous
    //  Q or q is quit ; alias string  = run test; ? write the menu
    //  # = run all tests
    //
    char linebuffer[120];
    gettestresponse(linebuffer);
    if ( strcmp(linebuffer,"Q") == 0) {
        return QUIT;     
    } else if ( strcmp(linebuffer,"q") == 0) {
        return QUIT;
    } else if ( strcmp(linebuffer,"#") == 0) {
        return ALL;
    } else if ( strcmp(linebuffer,"?") == 0) {
        writemenu();
        return getselectionid();
    } else if ( strcmp(linebuffer,"") == 0) {
        return singletestnumber;
    } else if ( strcmp(linebuffer,"+") == 0) {
        if (singletestnumber < testcount) {
            return ++singletestnumber;
        }
        return OUT_OF_RANGE;
    } else if ( strcmp(linebuffer,"-") == 0) {
        if (singletestnumber > 1) {
            return --singletestnumber;
        } 
        return OUT_OF_RANGE;
    } else if ( isint(linebuffer)) {
        int val = atoi(linebuffer);
        if (val>0 && val<=testcount) {
            return (singletestnumber = val);
        }
        return OUT_OF_RANGE;
    } else {
        for (int i=0; i < testcount; i++) {
            if (strcmp(linebuffer, tcbs[i].alias)== 0) {
                return (singletestnumber = i+1);
            }
        }
        return OUT_OF_RANGE;
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

bool add_test(char* name, char* alias, void (*testfunction)() ) {
    if (testcount >= MAXTESTS) {
        return false;
    }
    tcbs[testcount].name = name;
    tcbs[testcount].alias = alias;
    tcbs[testcount].passcount = 0;
    tcbs[testcount].failcount = 0;
    tcbs[testcount++].testfunction = testfunction;
    return true;
}

void ptest_execute() {
    puts("\nTESTING STARTING ...\n");
    if (test_if_selection_mode()){
        while (true) {
            int selectedtest = getselectionid();
            if (selectedtest == QUIT) break;
            if (selectedtest == OUT_OF_RANGE) {
                continue;
            } else if (selectedtest > 0) {
                execute_test(selectedtest);
                summary_of_test();
            } else if (selectedtest == ALL) {
                execute_tests();
                summary_of_tests();
            }
        }
    } else {
        execute_tests();
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
