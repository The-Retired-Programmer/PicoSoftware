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

#define MAXTESTS 20 

static struct test_control_block tcbs[MAXTESTS];
static uint testcount = 0;
static struct test_control_block *current_tcb;

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

void execute_tests() {
    for (uint i = 1; i <= testcount; i++) {
       execute_test(i);
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

//  and selective testing

uint singletestnumber=1;

bool test_if_selection_mode() {
    gpio_init(22);
    gpio_pull_up(22);
    return gpio_get(22)==0;
}

void gettestresponse(char* ptr) {
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

bool isint(char *line) {
    while(*line != '\0') {
        if (!isdigit(*line++)) {
            return false;
        }
    }
    return true;
}

void writemenu() {
    for (int i=0; i < testcount; i++) {
        printf("%s(%i) - %s\n", tcbs[i].alias, i+1, tcbs[i].name);
    }
}

int getselectionid() {
    // 1-n => test number ; 0 is  exit ; -1 is all
    // user command format: decimal digits (nn= test number or 0 = quit)
    //   + = next test number ; empty (ie just \n ) is repeat; - = previous
    //  Q or q is quit ; alias string  = run test; ? write the menu
    //  # = run all tests
    //
    char linebuffer[120];
    gettestresponse(linebuffer);
    if ( strcmp(linebuffer,"Q") == 0) {
        return 0;     
    } else if ( strcmp(linebuffer,"q") == 0) {
        return 0;
    } else if ( strcmp(linebuffer,"#") == 0) {

    } else if ( strcmp(linebuffer,"?") == 0) {
        writemenu();
        return getselectionid();
    } else if ( strcmp(linebuffer,"") == 0) {
        return singletestnumber;
    } else if ( strcmp(linebuffer,"+") == 0) {
        if (singletestnumber < testcount) {
            return ++singletestnumber;
        }
        return out_of_range;
    } else if ( strcmp(linebuffer,"-") == 0) {
        if (singletestnumber > 1) {
            return --singletestnumber;
        } 
        return out_of_range;
    } else if ( isint(linebuffer)) {
        int val = atoi(linebuffer);
        if (val>0 && val<=testcount) {
            return (singletestnumber = val);
        }
        return out_of_range;
    } else {
        for (int i=0; i < testcount; i++) {
            if (strcmp(linebuffer, tcbs[i].alias)== 0) {
                return (singletestnumber = i+1);
            }
        }
        return out_of_range;
    }
}

void execute_test(int selectionid) {
    current_tcb = &tcbs[selectionid-1];
    current_tcb->passcount = 0;
    current_tcb->failcount = 0;
    printf("RUNNING (%i) %s\n",selectionid, current_tcb->name);
    trace_init();
    current_tcb->testfunction();
    trace_print();
}

void summary_of_test() {
    if (tcbs[singletestnumber-1].failcount == 0) {
        puts("\nTEST STATUS: GREEN");
    } else {
        puts("\nTEST STATUS: RED");
    }
    printf("Checks: passed: %i, failed: %i;\n",
        tcbs[singletestnumber-1].passcount, tcbs[singletestnumber-1].failcount);
}

// ----------------------------------------------------------------------------
//
//  trace buffer for use by tests
//
// ----------------------------------------------------------------------------

#define TRACEBUFFERSIZE 1000
#define PRINTTRACELINESIZE 100
char tracebuffer[TRACEBUFFERSIZE];
uint insertpoint;
bool rotated;

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

void trace_print_segment(uint frompoint, uint size) {
    for (uint i = frompoint; i < frompoint+ size; i++) {
        putchar(tracebuffer[i]);
    }
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
