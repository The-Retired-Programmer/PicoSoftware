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
#include "hardware/timer.h"
#include "ptest.h"
#include "../src/logic_probe.h"
#include "../src/probe_controls.h"
#include "../src/frontend_commands.h"
#include "../src/digitalsampling.h"

static void commandaction(char *line) {
    puts(line);
    action_command(line);
}

static void it_commandaction(char* checkname, char *line, enum probestate expectedstate) {
    commandaction(line);
    pass_if_equal_uint(checkname, expectedstate,  getprobestate());
    is_probe_stop_complete();
}

static char *presamplecommands[] = { "p", "?", "g-command", "?", NULL};
static enum probestate presamplestates[] = {STATE_IDLE, STATE_IDLE, STATE_SAMPLING, STATE_SAMPLING};
static char *presamplechecknames[] ={"after p", "after p/?", "after g", "after g/?"};
static char *postsamplecommands[] = {"?", "d", "?" , NULL};
static enum probestate postsamplestates[] = {STATE_SAMPLING_DONE, STATE_IDLE, STATE_IDLE};
static char *postsamplechecknames[] = { "after s/?", "after d", "after d/?"};

static void itest_controller1(char *gcommand) { // mock for logic_analyser_controller()
    presamplecommands[2] = gcommand;
    char **nextcommand = presamplecommands;
    enum probestate *nextprobestate = presamplestates;
    char **nextcheckname = presamplechecknames;
    pass_if_equal_uint("initialstate", STATE_IDLE,  getprobestate());
    while (*nextcommand != NULL ) {
        it_commandaction(*nextcheckname++, *nextcommand++, *nextprobestate++);
    }
    while (!is_probe_stop_complete());
    nextcommand = postsamplecommands;
    nextprobestate = postsamplestates;
    nextcheckname = postsamplechecknames;
    while (*nextcommand != NULL ) {
        it_commandaction(*nextcheckname++, *nextcommand++, *nextprobestate++);
    }
}

static void integration_tester1(char *gcommand) {
    probe_init(); // as per src/main.c
    itest_controller1(gcommand ); // mock for frontend_commands_controller
    struct sample_buffers *samplebuffers = getsamplebuffers();
    printf("Sample Buffers: start at %i; count is %i\n",
        samplebuffers->earliest_valid_buffer,
        samplebuffers->valid_buffer_count);
}

static void itest_controller2(char *gcommand, uint32_t waitusec) { // mock for logic_analyser_controller()
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
    commandaction("s");
    pass_if_equal_uint("after s", STATE_STOPPING_SAMPLING,  getprobestate());
    while (!is_probe_stop_complete());
    nextcommand = postsamplecommands;
    nextprobestate = postsamplestates;
    nextcheckname = postsamplechecknames;
    while (*nextcommand != NULL ) {
        it_commandaction(*nextcheckname++, *nextcommand++, *nextprobestate++);
    }
}

static void integration_tester2(char *gcommand, uint32_t waitusec) {
    probe_init(); // as per src/main.c
    itest_controller2(gcommand, waitusec ); // mock for frontend_commands_controller
    struct sample_buffers *samplebuffers = getsamplebuffers();
    printf("Sample Buffers: start at %i; count is %i\n",
        samplebuffers->earliest_valid_buffer,
        samplebuffers->valid_buffer_count);
}

//  assuming sample size 3200, sample rate = 20K/s, sample width = 1
#define MICROSECS_PER_BUFFER 40000

static void integration_test1() {
    integration_tester1("g-16-1-19200-0-16-2-0-16-0-1-3200");
}

static void integration_test1a() {
    integration_tester2("g-16-1-19200-0-16-2-0-16-0-1-3200", MICROSECS_PER_BUFFER*2);
}

static void integration_test2() {
    integration_tester2("g-16-1-19200-0-16-2-0-16-0-0-3200", MICROSECS_PER_BUFFER/2);
}

static void integration_test3() {
    integration_tester2("g-16-1-19200-0-16-2-0-16-0-0-3200", MICROSECS_PER_BUFFER*3);
}

static void integration_test4() {
    integration_tester2("g-16-1-19200-0-16-2-0-16-0-0-3200", MICROSECS_PER_BUFFER*10);
}

// =============================================================================
//
// module API
//
// =============================================================================

void integration_test_init() {
    add_test("integration test - stop on buffer full", "it", integration_test1);
    add_test("integration test - interrupt on buffer full", "it", integration_test1a);
    add_test("integration test - manual stop - short wait", "it", integration_test2);
    add_test("integration test - manual stop - long wait", "it", integration_test3);
    add_test("integration test - manual stop - longer wait", "it", integration_test4);
}
