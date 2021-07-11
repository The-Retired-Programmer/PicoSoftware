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
// LOGIC ANALYSER PROBE - using Pico as Probe
//

#include "pio.h"
#include "storage.h"
#include "logic_probe.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"

// config variables
uint pinbase;
uint pinwidth;
uint32_t frequency;
bool st_enabled;
uint st_pin;
uint st_level;
bool ev_enabled;
uint ev_pin;
uint ev_level;
uint sampleendmode;
uint32_t samplesize;
//
enum probestate { STATE_IDLE, STATE_SAMPLING, STATE_SAMPLING_DONE } state = STATE_IDLE;

bool getUint(uint* varptr) {
    char* varstring = strtok(NULL,"-");
    if (varstring == NULL) {
        return false;
    }
    *varptr = (uint) atoi(varstring);
    return true;
}

bool getUint32(uint32_t* varptr) {
    char* varstring = strtok(NULL,"-");
    if (varstring == NULL) {
        return false;
    }
    *varptr = (uint32_t) atol(varstring);
    return true;
}

bool getBool(bool* varptr) {
    char* varstring = strtok(NULL,"-");
    if (varstring == NULL) {
        return false;
    }
    *varptr = varstring[0]=='1';
    return true;
}

bool parse_parameters(char* cmdbuffer) {
    // command format: <command>-<pinbase>-<pinwidth>-<frequency>\
    //                          -<startenabled>-<startpin>-<startlevel>\
    //                          -<eventenabled>-<eventpin>-<eventlevel>\
    //                          -<sampleendmode>-<samplesize>
    char* cmd = strtok(cmdbuffer,"-");
    return  getUint(&pinbase) && getUint(&pinwidth)&& getUint32(&frequency)
        && getBool(&st_enabled)&& getUint(&st_pin) && getUint(&st_level)
        && getBool(&ev_enabled)&& getUint(&ev_pin) && getUint(&ev_level)
        && getUint(&sampleendmode)&&getUint32(&samplesize);
}

//=============================================================================
//
// the "public" API for the probe
//
//=============================================================================

void probe_writestate() {
    printf("%i\n", state);
    puts(GOOD);
}

void probe_go(char* cmdbuffer) {
    if (state != STATE_IDLE) {
        puts("N illegal state");
        return; 
    }
    if (!parse_parameters(cmdbuffer) ) {
        puts("N command parse failure");
        return; 
    }
    storage_init(samplesize, pinbase, pinwidth);
    pio_init(pinbase, pinwidth, frequency);
    pio_arm(pinbase, false);
    storage_arm();
    //storage_waituntilcompleted();
    state = STATE_SAMPLING;
    puts(GOOD);
}

void probe_stop() {
    if (state != STATE_SAMPLING) {
        puts(BAD);
        return;
    }
    // needs to do stop action ; let the real stop change status
    //  *********for debug purposes - do the state change here *******
    state = STATE_SAMPLING_DONE; 
    puts(GOOD);
}

void probe_writesample() {
    if (state != STATE_SAMPLING_DONE) {
        puts(BAD);
        return;
    }
    puts(get_RLE_encoded_sample(pinbase));
    puts(GOOD);
    state = STATE_IDLE;
}

uint32_t *get_buf_wordptr(uint word_index) {
    uint pageindex = word_index/get_capturebuf_size();
    //hard_assert(pageindex < get_bufs_count());
    uint pageoffset = word_index%get_capturebuf_size();
    return get_capturebuf(pageindex); 

}
