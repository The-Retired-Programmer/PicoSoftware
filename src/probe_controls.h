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

#ifndef _PROBE_CONTROLS_H
#define _PROBE_CONTROLS_H

//
// LOGIC ANALYSER PROBE CONTROLS
//

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"

enum trigger_on { TRIGGER_ON_LOW, TRIGGER_ON_HIGH, TRIGGER_ON_FALL, TRIGGER_ON_RISE};
enum sample_end_mode {MANUAL, BUFFER_FULL, EVENT_BUFFERSTART, EVENT_BUFFEREND, EVENT_BUFFERMIDPOINT};
enum probestate { STATE_IDLE, STATE_SAMPLING, STATE_STOPPING_SAMPLING, STATE_SAMPLING_DONE };

struct probe_controls {
    uint pinbase;
    uint pinwidth;
    uint32_t frequency;
    bool st_enabled;
    uint st_pin;
    enum trigger_on st_trigger;
    bool ev_enabled;
    uint ev_pin;
    enum trigger_on ev_trigger;
    enum sample_end_mode sampleendmode;
    uint32_t samplesize;
    enum probestate state;
};

char* parse_control_parameters(struct probe_controls* controls, char* cmdline);

uint samplesperword(struct probe_controls* controls);

uint usedbitsperword(struct probe_controls* controls);

#ifdef TESTINGBUILD

bool parse_pinbase(struct probe_controls* controls, char* s);

char* get_errormessage();

#endif

#endif
