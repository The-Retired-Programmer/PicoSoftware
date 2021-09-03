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
// integration test - logic analyser
//

#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "ptest.h"
#include "../src/logic_probe.h"
#include "../src/logic_probe_internal.h"
#include "../src/frontend_commands_internal.h"
#include "../src/probe_controls.h"
#include "integration_test.h"

void integration_test1() {
    integration_test("g-16-1-19200-1-16-2-0-16-0-1-3200", 170000);
}

void integration_test2() {
    integration_test("g-16-1-19200-1-16-2-0-16-0-0-3200", 1000);
}

void integration_test3() {
    integration_test("g-16-1-19200-1-16-2-0-16-0-0-3200", 250000);
}

void integration_test_init() {
    add_test("integration test - stop on buffer full", "it", integration_test1);
    add_test("integration test - manual stop - short wait", "it", integration_test2);
    add_test("integration test - manual stop - long wait", "it", integration_test3);
}

void integration_test(char *gcommand, uint32_t waitusec) {
    probe_init(); // as per src/main.c
    itest_controller(gcommand, waitusec ); // mock for frontend_commands_controller
}

char *presamplecommands[] = { "p", "?", "g-command", "?", NULL};
enum probestate presamplestates[] = {STATE_IDLE, STATE_IDLE, STATE_SAMPLING, STATE_SAMPLING};
char *presamplechecknames[] ={"after p", "after p/?", "after g", "after g/?"};
char *postsamplecommands[] = {"?", "d", "?" , NULL};
enum probestate postsamplestates[] = {STATE_SAMPLING_DONE, STATE_IDLE, STATE_IDLE};
char *postsamplechecknames[] = { "after s/?", "after d", "after d/?"};

void itest_controller(char *gcommand, uint32_t waitusec) { // mock for logic_analyser_controller()
    presamplecommands[2] = gcommand;
    char **nextcommand = presamplecommands;
    enum probestate *nextprobestate = presamplestates;
    char **nextcheckname = presamplechecknames;
    pass_if_equal_uint("initialstate", STATE_IDLE,  getprobestate());
    while (*nextcommand != NULL ) {
        it_commandaction(*nextcheckname++, *nextcommand++, *nextprobestate++);
    }
    //
    // need to wait for a period
    //
    busy_wait_us_32(waitusec);
    action_command("s");
    pass_if_equal_uint("after s", STATE_STOPPING_SAMPLING,  getprobestate());
    while (!is_probe_stop_complete());
    nextcommand = postsamplecommands;
    nextprobestate = postsamplestates;
    nextcheckname = postsamplechecknames;
    while (*nextcommand != NULL ) {
        it_commandaction(*nextcheckname++, *nextcommand++, *nextprobestate++);
    }
}

void it_commandaction(char* checkname, char *line, enum probestate expectedstate) {
    char cmdbuffer[255];
    action_command(strcpy(cmdbuffer,line));
    pass_if_equal_uint(checkname, expectedstate,  getprobestate());
    is_probe_stop_complete();
}