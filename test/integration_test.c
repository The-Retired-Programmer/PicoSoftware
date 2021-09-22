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
#include <string.h>
#include "hardware/timer.h"
#include "ptest.h"
#include "../src/logic_probe.h"
#include "../src/probe_controls.h"
#include "../src/frontend_commands.h"
#include "../src/digitalsampling.h"
#include "../src/square_wave_generator.h"
#include "../src/gpio_probe_event.h"


//  assuming sample size 32000, sample rate = 1M/s, sample width = 1
#define MICROSECS_PER_BUFFER 8000
// small amount  of time to ensure we don't clash with a buffer boundary
#define DELTA (MICROSECS_PER_BUFFER/10)

#define MAXLINES 3
static char *receivedresponse;
static bool ismultilineresponse;
static char multilineresponse[MAXLINES][256];
static int responselines;

static int it_response_puts(char *response) {
    if (ismultilineresponse) {
        if (responselines < MAXLINES) strcpy(multilineresponse[responselines++], response);
    } else {
        receivedresponse = response;
    }
}

static int it_ack_puts() {
    pass("command - successfully processed");
}

static int it_nak_puts(char *response) {
    fail(response);
}

static void it_commandaction(char* checkname, char *line, enum probestate expectedstate, char *expectedresponse) {
    if (ismultilineresponse = (strcmp(expectedresponse, "<MULTILINE>")==0)){
        responselines=0;
        action_command(line);
    } else {
        receivedresponse="";
        action_command(line);
        pass_if_equal_string("expected response", expectedresponse, receivedresponse);
    }
    pass_if_equal_uint(checkname, expectedstate,  getprobestate());
    is_probe_stop_complete();
}

static void check_data_is_for_pin(uint expectedpin, char* id_line) {
    int pin;
    pass_if_equal_int("pin header in data", 1, sscanf(id_line, "# %i", &pin));
    pass_if_equal_int("expected d pin response", expectedpin, pin);
}

static void check_data_length(uint expectedbuffercount, char* data_line) {
    struct sample_buffers *samplebuffers = getsamplebuffers();
    uint bitsperbuffer = samplebuffers->buffer_size_words*32;
    int databitcount;
    pass_if_equal_int("data length (bits) format", 1, sscanf(data_line, "%iL", &databitcount));
    pass_if_equal_int("data length (bits)", bitsperbuffer*expectedbuffercount, databitcount);
}

static void check_data_length_and_split(uint expectedbuffercount, char* data_line) {
    struct sample_buffers *samplebuffers = getsamplebuffers();
    uint bitsperbuffer = samplebuffers->buffer_size_words*32;
    int zerodatabitcount;
    int onedatabitcount;
    pass_if_equal_int("data length (bits) format", 2, sscanf(data_line, "%iL%iH", &zerodatabitcount, &onedatabitcount));
    pass_if_equal_int("total data length (bits)", bitsperbuffer*expectedbuffercount, zerodatabitcount+onedatabitcount);
}

static void check_buffer_content_on_event(uint pin, uint expectedbuffercount) {
    if (responselines == 2) {
        check_data_is_for_pin(pin, multilineresponse[0]);
        check_data_length_and_split(expectedbuffercount, multilineresponse[1]);
    } else {
        fail("multiline response has incorrect length");
    }
}

static void check_buffer_content_on_stop(uint pin, uint expectedbuffercount) {
    if (responselines == 2) {
        check_data_is_for_pin(pin, multilineresponse[0]);
        check_data_length(expectedbuffercount, multilineresponse[1]);
    } else {
        fail("multiline response has incorrect length");
    }
}

static void check_buffer_content_on_completion(uint pin, uint expectedbuffercount) {
    if (responselines == 2) {
        check_data_is_for_pin(pin, multilineresponse[0]);
        check_data_length(expectedbuffercount, multilineresponse[1]);
    } else {
        fail("multiline response has incorrect length");
    }
}

static char *presamplecommands[] = { "p", "?", "g-command", "?", NULL};
static enum probestate presamplestates[] = {STATE_IDLE, STATE_IDLE, STATE_SAMPLING, STATE_SAMPLING};
static char *presamplechecknames[] ={"after p", "after p/?", "after g", "after g/?"};
static char *presampleexpectedresponse[] = {"PICO-1", "0", "", "1"};

static char *postsamplecommands[] = {"?", "d", "?" , NULL};
static enum probestate postsamplestates[] = {STATE_SAMPLING_DONE, STATE_IDLE, STATE_IDLE};
static char *postsamplechecknames[] = { "after done", "after d", "after d/?"};
static char *postsampleexpectedresponse[] = {"3", "<MULTILINE>", "0"};

static void run_commands(char **commands, enum probestate *states, char **checknames, char **expectedresponse) {
    while (*commands != NULL ) {
        it_commandaction(*checknames++, *commands++, *states++, *expectedresponse++);
    }
}

static void setup_and_start(char *gcommand) {
    presamplecommands[2] = gcommand;
    pass_if_equal_uint("initialstate", STATE_IDLE,  getprobestate());
    run_commands(presamplecommands, presamplestates, presamplechecknames, presampleexpectedresponse);
}

static void wait_and_completion(uint expectedstartbuffer, uint expectedbuffercount) {
    while (!is_probe_stop_complete());
    run_commands(postsamplecommands, postsamplestates, postsamplechecknames, postsampleexpectedresponse);
    struct sample_buffers *samplebuffers = getsamplebuffers();
    printf("Sample Buffers: start at %i; count is %i\n",
        samplebuffers->earliest_valid_buffer,
        samplebuffers->valid_buffer_count);
    pass_if_equal_uint("start buffer", expectedstartbuffer, samplebuffers->earliest_valid_buffer);
    pass_if_equal_uint("buffer count", expectedbuffercount, samplebuffers->valid_buffer_count);
}

static void run_to_completion(char *gcommand, uint expectedstartbuffer, uint expectedbuffercount, uint data_pin) {
    probe_init(it_response_puts, it_ack_puts, it_nak_puts); // as per src/main.c
    square_wave_generator_init(19,125000);
    square_wave_generator_start();
    setup_and_start(gcommand);
    wait_and_completion(expectedstartbuffer, expectedbuffercount);
    check_buffer_content_on_completion(data_pin, expectedbuffercount);
}

static void run_with_stop_command(char *gcommand, uint expectedstartbuffer, uint expectedbuffercount, uint32_t waitusec, uint data_pin) {
    probe_init(it_response_puts, it_ack_puts, it_nak_puts);
    square_wave_generator_init(19,125000);
    square_wave_generator_start();
    setup_and_start(gcommand);
    if (waitusec > 0) busy_wait_us_32(waitusec);
    action_command("s");
    pass_if_equal_uint("after s", STATE_STOPPING_SAMPLING,  getprobestate());
    wait_and_completion(expectedstartbuffer, expectedbuffercount);
    check_buffer_content_on_stop(data_pin, expectedbuffercount);
}

static void run_with_stop_on_event(char *gcommand, uint expectedstartbuffer, uint expectedbuffercount, uint32_t eventwait, uint event_pin) {
    probe_init(it_response_puts, it_ack_puts, it_nak_puts); // as per src/main.c
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
    check_buffer_content_on_event(event_pin, expectedbuffercount);
}

static void to_buffer_full() {
    run_to_completion("g-18-1-1000000-0-19-2-0-18-0-1-32000", 0, 4, 18);
}

static void stop_before_buffer_full() {
    run_with_stop_command("g-18-1-1000000-0-19-2-0-18-0-1-32000", 0, 1, DELTA, 18);
}

static void manual_stop_quick() {
    run_with_stop_command("g-18-1-1000000-0-19-2-0-18-0-0-32000", 0, 1, DELTA, 18);
}

static void manual_stop_medium() {
    run_with_stop_command("g-18-1-1000000-0-19-2-0-18-0-0-32000", 0,3, MICROSECS_PER_BUFFER*2+DELTA, 18);
}

static void manual_stop_long() {
    run_with_stop_command("g-18-1-1000000-0-19-2-0-18-0-0-32000", 3, 4, MICROSECS_PER_BUFFER*10+DELTA, 18);
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
