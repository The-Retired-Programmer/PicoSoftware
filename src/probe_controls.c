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
#include "probe_controls_internal.h"

char errormessage[256];
uint32_t lastint;


//------------------------------------------------------------------------------
//
//  PUBLIC API
//
//------------------------------------------------------------------------------

char* parse_control_parameters(struct probe_controls* controls, char* cmdbuffer) {
    sprintf(errormessage,"%s","Unknown error");
    char* cmd = strtok(cmdbuffer,"-");
    return  parse_pinbase(controls, strtok(NULL,"-")) &&
            parse_pinwidth(controls, strtok(NULL,"-")) &&
            parse_frequency(controls, strtok(NULL,"-")) &&
            parse_startenabled(controls, strtok(NULL,"-")) &&
            parse_startpin(controls, strtok(NULL,"-")) &&
            parse_starttrigger(controls, strtok(NULL,"-")) &&
            parse_eventenabled(controls, strtok(NULL,"-")) &&
            parse_eventpin(controls, strtok(NULL,"-")) &&
            parse_eventtrigger(controls, strtok(NULL,"-")) &&
            parse_sampleendmode(controls, strtok(NULL,"-")) &&
            parse_samplesize(controls, strtok(NULL,"-"))
            ? NULL : errormessage; 
}

//------------------------------------------------------------------------------
//
//  PRIVATE FUNCTIONS
//
//------------------------------------------------------------------------------

char* get_errormessage() {
    return errormessage;
}

bool parse_pinbase(struct probe_controls* controls, char* s) {
    if (s == NULL) {
        sprintf(errormessage,"%s","missing base pin value");
        return false;
    }
    if (parse_int(s)){
        controls->pinbase = (uint) lastint;
        return true;
    } else {
        sprintf(errormessage,"%s (%s)","illegal integer - base pin value",s);
        return false;
    }
}

bool parse_pinwidth(struct probe_controls* controls, char* s) {
    if (s == NULL) {
        sprintf(errormessage,"%s","missing pin width value");
        return false;
    }
    if (parse_int(s)){
        controls->pinwidth = (uint) lastint;
        return true;
    } else {
        sprintf(errormessage,"%s (%s)","illegal integer - pin width value",s);
        return false;
    }
}

bool parse_frequency(struct probe_controls* controls, char* s) {
    if (s == NULL) {
        sprintf(errormessage,"%s","missing frequency value");
        return false;
    }
    if (parse_int(s)){
        controls->frequency = lastint;
        return true;
    } else {
        sprintf(errormessage,"%s (%s)","illegal integer - frequency value",s);
        return false;
    }
}

bool parse_startenabled(struct probe_controls* controls, char* s) {
    if (s == NULL) {
        sprintf(errormessage,"%s","missing start enabled value");
        return false;
    }
    if (parse_int(s)){
        controls->st_enabled = lastint == 1;
        return true;
    } else {
        sprintf(errormessage,"%s (%s)","illegal integer - start enabled value",s);
        return false;
    }
}

bool parse_startpin(struct probe_controls* controls, char* s) {
    if (s == NULL) {
        sprintf(errormessage,"%s","missing start pin value");
        return false;
    }
    if (parse_int(s)){
        controls->st_pin = (uint) lastint;
        return true;
    } else {
        sprintf(errormessage,"%s (%s)","illegal integer - start pin value",s);
        return false;
    }
}

bool parse_starttrigger(struct probe_controls* controls, char* s) {
    if (s == NULL) {
        sprintf(errormessage,"%s","missing start trigger value");
        return false;
    }
    if (parse_int(s)){
        controls->st_trigger = (enum trigger_on) lastint;
        return true;
    } else {
        sprintf(errormessage,"%s (%s)","illegal integer - start trigger value", s);
        return false;
    }
}

bool parse_eventenabled(struct probe_controls* controls, char* s) {
    if (s == NULL) {
        sprintf(errormessage,"%s","missing event enabled value");
        return false;
    }
    if (parse_int(s)){
        controls->ev_enabled = lastint == 1;
        return true;
    } else {
        sprintf(errormessage,"%s (%s)","illegal integer - event enabled value", s);
        return false;
    }
}

bool parse_eventpin(struct probe_controls* controls, char* s) {
    if (s == NULL) {
        sprintf(errormessage,"%s","missing event pin value");
        return false;
    }
    if (parse_int(s)){
        controls->ev_pin = (uint) lastint;
        return true;
    } else {
        sprintf(errormessage,"%s (%s)","illegal integer - event pin value", s);
        return false;
    }
}

bool parse_eventtrigger(struct probe_controls* controls, char* s) {
    if (s == NULL) {
        sprintf(errormessage,"%s","missing event trigger value");
        return false;
    }
    if (parse_int(s)){
        controls->ev_trigger = (enum trigger_on) lastint;
        return true;
    } else {
        sprintf(errormessage,"%s (%s)","illegal integer - event trigger value", s);
        return false;
    }
}

bool parse_sampleendmode(struct probe_controls* controls, char* s) {
    if (s == NULL) {
        sprintf(errormessage,"%s","missing sample end mode value");
        return false;
    }
    if (parse_int(s)){
        controls->sampleendmode = (enum sample_end_mode) lastint;
        return true;
    } else {
        sprintf(errormessage,"%s (%s)","illegal integer - sample end mode value", s);
        return false;
    }
}

bool parse_samplesize(struct probe_controls* controls, char* s) {
    if (s == NULL) {
        sprintf(errormessage,"%s","missing sample size value");
        return false;
    }
    if (parse_int(s)){
        controls->samplesize = lastint;
        return true;
    } else {
        sprintf(errormessage,"%s (%s)","illegal integer - sample size value", s);
        return false;
    }
}

bool parse_int(char* s) {
    lastint = 0;
    while (true) {
        char c = *s++;
        if (c == '\0') {
            return true;
        } else if ( c == ' ' ) {
            // skip any spaces
        } else if ( c >= '0' && c <= '9') {
            lastint = lastint*10 + (uint32_t) (c - '0');
        } else {
            return false;
        }
    }
}

