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
// LOGIC ANALYSER PROBE - command actions
//

#include "hardware/gpio.h"
#include "digitalsampling.h"
#include "gpio_probe_event.h"
#include "run_length_encoder.h"
#include "square_wave_generator.h"
#include "probe_controls.h"
#include <stdio.h>

struct probe_controls probecontrols;
int (*_responsewriter)(const char* response);
int (*_ack_responsewriter)();
int (*_nak_responsewriter)(const char* response);
char response_buffer[256];

static void probe_pass_teardown() {
    digitalsampling_teardown();
}

static void probe_pass_reset(char *response) {
    probe_pass_teardown();
    probecontrols.state = STATE_IDLE;
    _nak_responsewriter(response);
}

// =============================================================================
//
// module API
//
// =============================================================================

void probe_init(int (*responsewriter)(const char* response), int (*ack_responsewriter)(), int (*nak_responsewriter)(const char* response)) {
    probecontrols.state = STATE_IDLE;
    _responsewriter = responsewriter;
    _ack_responsewriter = ack_responsewriter;
    _nak_responsewriter = nak_responsewriter;
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
}

void probe_ping() {
    _responsewriter("PICO-1");
    _ack_responsewriter();
}

void probe_flash(char* cmdbuffer) {
    int fparam;
    if (sscanf(cmdbuffer, "f-%d", &fparam) == 1) {
        gpio_put(PICO_DEFAULT_LED_PIN, fparam);
        _ack_responsewriter();
    } else {
        sprintf(response_buffer, "parse failure (f command) - %s", cmdbuffer);
        probe_pass_reset(response_buffer);
    }
}

void probe_waveform(char* cmdbuffer) {
    int enable;
    int pin_base;
    float frequency;
    if (sscanf(cmdbuffer, "w-%d-%d-%f", &enable, &pin_base, &frequency) == 3) {
        if (enable) {
            char* errmsg = square_wave_generator_init(pin_base, frequency);
            if (errmsg != NULL) {
                sprintf(response_buffer, "initiation failure - %s",errmsg);
                probe_pass_reset(response_buffer);
                return;
            }
            square_wave_generator_start();
            _ack_responsewriter();
        } else {
            teardown_square_wave_generator();
        }
    } else {
        sprintf(response_buffer, "parse failure (w command) - %s", cmdbuffer);
        probe_pass_reset(response_buffer);
    }
}

void probe_getstate() {
    sprintf(response_buffer, "%i", probecontrols.state);
    _responsewriter(response_buffer);
    _ack_responsewriter();
}

void probe_go(char* cmdbuffer) {
    if (probecontrols.state != STATE_IDLE) {
        sprintf(response_buffer, "Bad state - expecting STATE_IDLE(0) - was %i", probecontrols.state);
        probe_pass_reset(response_buffer);
        return; 
    }
    char* errormessage = parse_control_parameters(&probecontrols, cmdbuffer);
    if (errormessage != NULL ) {
        sprintf(response_buffer, "Command parse failure - %s",errormessage);
        probe_pass_reset(response_buffer);
        return; 
    }
    errormessage = digitalsampling_start(&probecontrols);
    if (errormessage != NULL ) {
        sprintf(response_buffer, "Failure initiating digital sampling- %s",errormessage);
        probe_pass_reset(response_buffer);
        return; 
    }
    probecontrols.state = STATE_SAMPLING;
    _ack_responsewriter();
}

void probe_stop() {
    if (probecontrols.state != STATE_SAMPLING) {
        sprintf(response_buffer, "Bad state - expecting STATE_SAMPLING(1) - was %i", probecontrols.state);
        probe_pass_reset(response_buffer);
        return;
    }
    digitalsampling_stop();
    probecontrols.state = STATE_STOPPING_SAMPLING;
    _ack_responsewriter();
}

volatile bool is_probe_stop_complete() {
    if (probecontrols.state == STATE_SAMPLING && has_event_triggered()) probecontrols.state = STATE_STOPPING_SAMPLING;
    if ((probecontrols.state == STATE_SAMPLING || probecontrols.state == STATE_STOPPING_SAMPLING) && is_digitalsampling_finished()) probecontrols.state = STATE_SAMPLING_DONE;
    return probecontrols.state == STATE_SAMPLING_DONE;
}

void probe_getsample() {
    if (probecontrols.state != STATE_SAMPLING_DONE) {
        sprintf(response_buffer, "Bad state - expecting STATE_SAMPLING_DONE(3) - was %i", probecontrols.state);
        probe_pass_reset(response_buffer);
        return;
    }
    create_RLE_encoded_sample(&probecontrols, getsamplebuffers(), _responsewriter);
    probecontrols.state = STATE_IDLE;
    probe_pass_teardown();
    _ack_responsewriter();
}

#ifdef TESTINGBUILD

enum probestate getprobestate() {
    return probecontrols.state;
}

#endif

