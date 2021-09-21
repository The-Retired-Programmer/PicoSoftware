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

#include <stdlib.h>
#include "hardware/timer.h"
#include "ptest.h"
#include "../src/logic_probe.h"
#include "../src/probe_controls.h"
#include "../src/frontend_commands.h"
#include "../src/digitalsampling.h"
#include "../src/square_wave_generator.h"
#include "../src/gpio_probe_event.h"

int it_ack_puts() {
    puts("Y");
}

int it_nak_puts(char *response) {
    fail(response);
}

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
static char *postsamplechecknames[] = { "after done", "after d", "after d/?"};

static void run_commands(char **commands, enum probestate *states, char **checknames) {
    while (*commands != NULL ) {
        it_commandaction(*checknames++, *commands++, *states++);
    }
}

static void setup_and_start(char *gcommand) {
    presamplecommands[2] = gcommand;
    pass_if_equal_uint("initialstate", STATE_IDLE,  getprobestate());
    run_commands(presamplecommands, presamplestates, presamplechecknames);
}

static void wait_and_completion(uint expectedstartbuffer, uint expectedbuffercount) {
    while (!is_probe_stop_complete());
    run_commands(postsamplecommands, postsamplestates, postsamplechecknames);
    struct sample_buffers *samplebuffers = getsamplebuffers();
    printf("Sample Buffers: start at %i; count is %i\n",
        samplebuffers->earliest_valid_buffer,
        samplebuffers->valid_buffer_count);
    pass_if_equal_uint("start buffer", expectedstartbuffer, samplebuffers->earliest_valid_buffer);
    pass_if_equal_uint("buffer count", expectedbuffercount, samplebuffers->valid_buffer_count);
}

static void run_to_completion(char *gcommand, uint expectedstartbuffer, uint expectedbuffercount) {
    probe_init(puts, it_ack_puts, it_nak_puts); // as per src/main.c
    square_wave_generator_init(19,125000);
    square_wave_generator_start();
    setup_and_start(gcommand);
    wait_and_completion(expectedstartbuffer, expectedbuffercount);
}

static void run_with_stop_command(char *gcommand, uint expectedstartbuffer, uint expectedbuffercount, uint32_t waitusec) {
    probe_init(puts, it_ack_puts, it_nak_puts);
    square_wave_generator_init(19,125000);
    square_wave_generator_start();
    setup_and_start(gcommand);
    if (waitusec > 0) busy_wait_us_32(waitusec);
    commandaction("s");
    pass_if_equal_uint("after s", STATE_STOPPING_SAMPLING,  getprobestate());
    wait_and_completion(expectedstartbuffer, expectedbuffercount);  
}

//  assuming sample size 32000, sample rate = 1M/s, sample width = 1
#define MICROSECS_PER_BUFFER 8000
// small amount  of time to ensure we don't clash with a buffer boundary
#define DELTA (MICROSECS_PER_BUFFER/10)


static void run_with_stop_on_event(char *gcommand, uint expectedstartbuffer, uint expectedbuffercount, uint32_t eventwait, uint event_pin) {
    probe_init(puts, it_ack_puts, it_nak_puts); // as per src/main.c
    square_wave_generator_init(19,125000);
    square_wave_generator_start();
    setup_and_start(gcommand);
    gpio_set_dir(event_pin,true);
    gpio_put(event_pin, false);
    busy_wait_us_32(eventwait);
    gpio_put(event_pin, true);
    is_probe_stop_complete();
    pass_if_equal_uint("after event", STATE_STOPPING_SAMPLING,  getprobestate());
    wait_and_completion(expectedstartbuffer, expectedbuffercount);  
}

static void to_buffer_full() {
    run_to_completion("g-18-1-1000000-0-19-2-0-18-0-1-32000", 0, 4);
}

static void stop_before_buffer_full() {
    run_with_stop_command("g-18-1-1000000-0-19-2-0-18-0-1-32000", 0, 1, DELTA);
}

static void manual_stop_quick() {
    run_with_stop_command("g-18-1-1000000-0-19-2-0-18-0-0-32000", 0, 1, DELTA);
}

static void manual_stop_medium() {
    run_with_stop_command("g-18-1-1000000-0-19-2-0-18-0-0-32000", 0,3, MICROSECS_PER_BUFFER*2+DELTA);
}

static void manual_stop_long() {
    run_with_stop_command("g-18-1-1000000-0-19-2-0-18-0-0-32000", 3, 4, MICROSECS_PER_BUFFER*10+DELTA);
}

static void event_w1_immediate_event() {
    run_with_stop_on_event("g-18-1-1000000-0-19-2-1-18-3-2-32000", 0, 4, DELTA, 18);
}

static void event_w2_immediate_event() {
    run_with_stop_on_event("g-18-1-1000000-0-19-2-1-18-3-3-32000", 0, 4, DELTA, 18);
}

static void event_w3_immediate_event() {
    run_with_stop_on_event("g-18-1-1000000-0-19-2-1-18-3-4-32000", 0, 4, DELTA, 18);
}

static void event_w4_immediate_event() {
    run_with_stop_on_event("g-18-1-1000000-0-19-2-1-18-3-5-32000", 0, 4, DELTA, 18);
}

static void event_w1_b2_event() {
    run_with_stop_on_event("g-18-1-1000000-0-19-2-1-18-3-2-32000", 1, 4, MICROSECS_PER_BUFFER+DELTA, 18);
}

static void event_w2_b2_event() {
    run_with_stop_on_event("g-18-1-1000000-0-19-2-1-18-3-3-32000", 0, 4,MICROSECS_PER_BUFFER+DELTA, 18);
}

static void event_w3_b2_event() {
    run_with_stop_on_event("g-18-1-1000000-0-19-2-1-18-3-4-32000", 0, 4, MICROSECS_PER_BUFFER+DELTA, 18);
}

static void event_w4_b2_event() {
    run_with_stop_on_event("g-18-1-1000000-0-19-2-1-18-3-5-32000", 0, 4, MICROSECS_PER_BUFFER+DELTA, 18);
}

static void event_w1_b3_event() {
    run_with_stop_on_event("g-18-1-1000000-0-19-2-1-18-3-2-32000", 2, 4, 2*MICROSECS_PER_BUFFER+DELTA, 18);
}

static void event_w2_b3_event() {
    run_with_stop_on_event("g-18-1-1000000-0-19-2-1-18-3-3-32000", 1, 4,2*MICROSECS_PER_BUFFER+DELTA, 18);
}

static void event_w3_b3_event() {
    run_with_stop_on_event("g-18-1-1000000-0-19-2-1-18-3-4-32000", 0, 4, 2*MICROSECS_PER_BUFFER+DELTA, 18);
}

static void event_w4_b3_event() {
    run_with_stop_on_event("g-18-1-1000000-0-19-2-1-18-3-5-32000", 0, 4, 2*MICROSECS_PER_BUFFER+DELTA, 18);
}

static void event_w1_b4_event() {
    run_with_stop_on_event("g-18-1-1000000-0-19-2-1-18-3-2-32000", 3, 4, 3*MICROSECS_PER_BUFFER+DELTA, 18);
}

static void event_w2_b4_event() {
    run_with_stop_on_event("g-18-1-1000000-0-19-2-1-18-3-3-32000", 2, 4,3*MICROSECS_PER_BUFFER+DELTA, 18);
}

static void event_w3_b4_event() {
    run_with_stop_on_event("g-18-1-1000000-0-19-2-1-18-3-4-32000", 1, 4, 3*MICROSECS_PER_BUFFER+DELTA, 18);
}

static void event_w4_b4_event() {
    run_with_stop_on_event("g-18-1-1000000-0-19-2-1-18-3-5-32000", 0, 4, 3*MICROSECS_PER_BUFFER+DELTA, 18);
}

static void event_w1_long_event() {
    run_with_stop_on_event("g-18-1-1000000-0-19-2-1-18-3-2-32000", 0, 4, 8*MICROSECS_PER_BUFFER+DELTA, 18);
}

static void event_w2_long_event() {
    run_with_stop_on_event("g-18-1-1000000-0-19-2-1-18-3-3-32000", 3, 4, 8*MICROSECS_PER_BUFFER+DELTA, 18);
}

static void event_w3_long_event() {
    run_with_stop_on_event("g-18-1-1000000-0-19-2-1-18-3-4-32000", 2, 4, 8*MICROSECS_PER_BUFFER+DELTA, 18);
}

static void event_w4_long_event() {
    run_with_stop_on_event("g-18-1-1000000-0-19-2-1-18-3-5-32000", 1, 4, 8*MICROSECS_PER_BUFFER+DELTA, 18);
}

// =============================================================================
//
// module API
//
// =============================================================================

void integration_test_init() {
    add_test("integration - stop on buffer full", "i", to_buffer_full);
    add_test("integration - interrupt on buffer full", "i", stop_before_buffer_full);
    add_test("integration - manual stop - short wait", "i", manual_stop_quick);
    add_test("integration - manual stop - long wait", "i", manual_stop_medium);
    add_test("integration - manual stop - longer wait", "i", manual_stop_long);
    add_test("integration - event stop - window 1 - immediate", "i", event_w1_immediate_event);
    add_test("integration - event stop - window 2 - immediate", "i", event_w2_immediate_event);
    add_test("integration - event stop - window 3 - immediate", "i", event_w3_immediate_event);
    add_test("integration - event stop - window 4 - immediate", "i", event_w4_immediate_event);
    add_test("integration - event stop - window 1 - b2", "i", event_w1_b2_event);
    add_test("integration - event stop - window 2 - b2", "i", event_w2_b2_event);
    add_test("integration - event stop - window 3 - b2", "i", event_w3_b2_event);
    add_test("integration - event stop - window 4 - b2", "i", event_w4_b2_event);
    add_test("integration - event stop - window 1 - b3", "i", event_w1_b3_event);
    add_test("integration - event stop - window 2 - b3", "i", event_w2_b3_event);
    add_test("integration - event stop - window 3 - b3", "i", event_w3_b3_event);
    add_test("integration - event stop - window 4 - b3", "i", event_w4_b3_event);
    add_test("integration - event stop - window 1 - b4", "i", event_w1_b4_event);
    add_test("integration - event stop - window 2 - b4", "i", event_w2_b4_event);
    add_test("integration - event stop - window 3 - b4", "i", event_w3_b4_event);
    add_test("integration - event stop - window 4 - b4", "i", event_w4_b4_event);
    add_test("integration - event stop - window 1 - long", "i", event_w1_long_event);
    add_test("integration - event stop - window 2 - long", "i", event_w2_long_event);
    add_test("integration - event stop - window 3 - long", "i", event_w3_long_event);
    add_test("integration - event stop - window 4 - long", "i", event_w4_long_event);
}
