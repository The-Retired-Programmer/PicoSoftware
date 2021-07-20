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
// LOGIC ANALYSER PROBE CONTROLS
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "probe_controls.h"

int testnumber = 0;
char* cmd;

bool getUint(uint* varptr, char* s) {
    testnumber++;
    if (s == NULL) {
        return false;
    }
    *varptr = (uint) atoi(s);
    return true;
}

bool getUint32(uint32_t* varptr, char* s) {
    testnumber++;
    if (s == NULL) {
        return false;
    }
    *varptr = (uint32_t) atol(s);
    return true;
}

bool getBool(bool* varptr, char* s) {
    testnumber++;
    if (s == NULL) {
        return false;
    }
    *varptr = s[0]=='1';
    return true;
}

bool getTrigger(enum trigger_on* varptr, char* s) {
    testnumber++;
    if (s == NULL) {
        return false;
    }
    *varptr = (enum trigger_on) atoi(s);
    return true;
}

bool getSampleEndMode(enum sample_end_mode* varptr, char* s) {
    testnumber++;
    if (s == NULL) {
        return false;
    }
    *varptr = (enum sample_end_mode) atoi(s);
    return true;
}

bool parse_control_parameters(struct probe_controls* controls, char* cmdbuffer) {
    // command format: <command>-<pinbase>-<pinwidth>-<frequency>\
    //                          -<startenabled>-<startpin>-<startlevel>\
    //                          -<eventenabled>-<eventpin>-<eventlevel>\
    //                          -<sampleendmode>-<samplesize>
    cmd = strtok(cmdbuffer,"-");
    testnumber = 0;
    return  getUint(&controls->pinbase, strtok(cmdbuffer,"-")) &&
            getUint(&controls->pinwidth, strtok(cmdbuffer,"-")) &&
            getUint32(&controls->frequency, strtok(cmdbuffer,"-")) &&
            getBool(&controls->st_enabled, strtok(cmdbuffer,"-")) &&
            getUint(&controls->st_pin, strtok(cmdbuffer,"-")) &&
            getTrigger(&controls->st_trigger, strtok(cmdbuffer,"-")) &&
            getBool(&controls->ev_enabled, strtok(cmdbuffer,"-")) &&
            getUint(&controls->ev_pin, strtok(cmdbuffer,"-")) &&
            getTrigger(&controls->ev_trigger, strtok(cmdbuffer,"-")) &&
            getSampleEndMode(&controls->sampleendmode, strtok(cmdbuffer,"-")) &&
            getUint32(&controls->samplesize, strtok(cmdbuffer,"-"));
}

int getTestnumber() {
    return testnumber;
}

char* getCmd() {
    return cmd;
}
