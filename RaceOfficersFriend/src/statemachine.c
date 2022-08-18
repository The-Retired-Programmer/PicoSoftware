/**
 * Copyright 2022 Richard Linsdale (richard at theretiredprogrammer.uk).
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
#include "pico/stdlib.h"
#include <stdio.h>
#include "statemachine.h"
#include "timezoneUI.h"
#include "statuszoneUI.h"
#include "graphiczoneUI.h"
#include "racingscreenUI.h"
#include "miscUI.h"
#include "zonelayout.h"
#include "screen.h"

/*
void onInit() {
    printf("Race Officer's friend - running\n");
    screenBegin();
    drawSplashScreen();
}

void onStart() {
    clearScreen(BLACK,FONT24PT);
    timezoneBegin();
    statuszoneBegin(statusZone);
    graphiczoneBegin();
    statuszoneWrite("[left] start");
}

void onExit() {
    timezoneEnd();
    statuszoneEnd(statusZone);
    graphiczoneEnd();
    sleep_ms(1000);
    screenEnd();
    printf("Race Officer's friend - shut down\n");
}

enum StateMachineEvent { TICK, BUTTON1, BUTTON2, BUTTON3 };

*/

//----------------------------------------------------------
enum State {INIT, STOP, WAITTORESTART, COUNTDOWN6, COUNTDOWN515,COUNTDOWN5, COUNTDOWN415, COUNTDOWN4,
        COUNTDOWN115, COUNTDOWN1, COUNTDOWN015, COUNTUP};

typedef struct action {
    void (*action)();
} SM_action;


// time management routines

uint16_t tickcounter; //0.5sec time counter - 9+ hours capacity

uint16_t time(uint16_t mins, uint16_t secs) {
    return (mins*60 + secs)*2;
}

int settime(uint16_t mins, uint16_t secs) {
    tickcounter = time(mins, secs);
}

int timeEquals(uint16_t mins, uint16_t secs) {
    return tickcounter == time(mins, secs);
}

bool onSecondTick() {
    return (tickcounter&0x1) == 0;
}

uint16_t mins() {
    return tickcounter/(60*2);
}

uint16_t secs() {
    return (tickcounter>>1)%60;
}

//  state machine actions

enum State state = INIT;
StateMachineEvent event;

void sm_ignore() {}

void sm_panic() {
    printf("STATE MACHINE PANIC - state = %i; event = %i; STOP", state,event);
    state = STOP;
}

void sm_init_tick() {
    state = COUNTDOWN6;
    tickcounter = time(5,20);
    timezoneTickdown(5,20);
    graphiczoneInit();
    statuszoneWrite("[right] postpone");
}

void sm_postpone() {
    // beep twice
    state = WAITTORESTART;
    timezoneTickdown(6,20);
    graphiczoneBegin();
    graphiczoneInit();
    statuszoneWrite("[left] restart");
}

void sm_init_restart() {
    // beep once
    drawStartScreen();
    state = COUNTDOWN6;
    tickcounter = time(6,20);
    timezoneTickdown(6,20);
    graphiczoneInit();
    statuszoneWrite("[right] postpone");
}

void sm_tickdownTimerto515() {
    tickcounter--;
    if (onSecondTick()) {
        timezoneTickdown(mins(), secs());
    }
    if (timeEquals(5,16)) {
        state = COUNTDOWN515;
    }
}

void sm_tickdownTimerWarningUp() {
    tickcounter--;
    if (timeEquals(5,0)) {
        state = COUNTDOWN5;
        warningFlagUp();
        timezoneTickdown(mins(), secs());
    } else {
        if (onSecondTick()) {
            timezoneTickdown(mins(), secs());
            warningFlagUpWarning(secs());
        }
    }
}

void sm_tickdownTimerto415() {
    tickcounter--;
    if (onSecondTick()) {
        timezoneTickdown(mins(), secs());
    }
    if (timeEquals(4,16)) {
        state = COUNTDOWN415;
    }
}

void sm_tickdownTimerPrepUp() {
    tickcounter--;
    if (timeEquals(4,0)) {
        state = COUNTDOWN4;
        prepFlagUp();
        timezoneTickdown(mins(), secs());
    } else {
        if (onSecondTick()) {
            timezoneTickdown(mins(), secs());
            prepFlagUpWarning(secs());
        }
    }
}

void sm_tickdownTimerto115() {
    tickcounter--;
    if (onSecondTick()) {
        timezoneTickdown(mins(), secs());
    }
    if (timeEquals(1,16)) {
        state = COUNTDOWN115;
    }
}

void sm_tickdownTimerPrepDown() {
    tickcounter--;
    if (timeEquals(1,0)) {
        state = COUNTDOWN1;
        prepFlagDown();
        timezoneTickdown(mins(), secs());
    } else {
        if (onSecondTick()) {
            timezoneTickdown(mins(), secs());
            prepFlagDownWarning(secs());
        }
    }
}

void sm_tickdownTimerto015() {
    tickcounter--;
    if (onSecondTick()) {
        timezoneTickdown(mins(), secs());
    }
    if (timeEquals(0,16)) {
        state = COUNTDOWN015;
    }
}

void sm_tickdownTimerWarningDown() {
    tickcounter--;
    if (timeEquals(0,0)) {
        state = COUNTUP;
        racingscreenBegin();
        racingscreenTickup(0,0);
        statuszoneBegin(racingStatusZone);
        statuszoneWrite("[right] general recall");
    } else {
        if (onSecondTick()) {
            timezoneTickdown(mins(), secs());
            warningFlagDownWarning(secs());
        }
    }
}

void sm_tickup(){
    tickcounter++;
    if (onSecondTick()) {
        racingscreenTickup(mins(), secs());
    }
}

SM_action statetable[12][4] = {
    // INIT
    { sm_ignore,sm_init_tick,sm_ignore,sm_postpone},
    // STOP
    { sm_panic,sm_panic,sm_panic,sm_panic},
    // WAITINGTORESTART
    { sm_ignore,sm_init_restart,sm_ignore,sm_ignore},
    // COUNTDOWN6
    { sm_tickdownTimerto515,sm_ignore,sm_ignore,sm_postpone},
    // COUNTDOWN515
    { sm_tickdownTimerWarningUp,sm_ignore,sm_ignore,sm_postpone},
    // COUNTDOWN5
    { sm_tickdownTimerto415,sm_ignore,sm_ignore,sm_postpone},
    // COUNTDOWN415
    { sm_tickdownTimerPrepUp,sm_ignore,sm_ignore,sm_postpone},
    // COUNTDOWN4
    { sm_tickdownTimerto115,sm_ignore,sm_ignore,sm_postpone},
    // COUNTDOWN115
    { sm_tickdownTimerPrepDown,sm_ignore,sm_ignore,sm_postpone},
    // COUNTDOWN1
    { sm_tickdownTimerto015,sm_ignore,sm_ignore,sm_postpone},
    // COUNTDOWN15
    { sm_tickdownTimerWarningDown,sm_ignore,sm_ignore,sm_postpone},
    // COUNTUP
    { sm_tickup,sm_ignore,sm_ignore,sm_ignore}
};

/* ----------------------------------------------------------
 *   API 
 */

void statemachineAction(StateMachineEvent evt) {
    event = evt;
    (*statetable[state][evt].action)();
}
