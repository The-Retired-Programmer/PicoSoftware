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
#include "timezoneUI.h"
#include "statuszoneUI.h"
#include "graphiczoneUI.h"
#include "miscUI.h"
#include "screen.h"

void onInit() {
    printf("Race Officer's friend - running\n");
    screenBegin();
    drawSplashScreen();
}

void onStart() {
    clearScreen(BLACK,FONT24PT);
    timezoneBegin();
    statuszoneBegin();
    graphiczoneBegin();
    statuszoneWrite("[left] start");
}

void onExit() {
    timezoneEnd();
    statuszoneEnd();
    graphiczoneEnd();
    sleep_ms(1000);
    screenEnd();
    printf("Race Officer's friend - shut down\n");
}

enum Event { TICK, BUTTON1, BUTTON2, BUTTON3 };

#define EVENTQSIZE 20
enum Event eventq[EVENTQSIZE];
uint eventinsert = 0;
uint eventextract = 0;

void insertEvent(enum Event event) {
    if (eventinsert == eventextract) {
        eventinsert = eventextract = 0;
    }
    if (eventinsert == EVENTQSIZE) {
        printf("FAIL - Event Buffer Overflow\n");
        printf("eventinsert=%i; eventextract=%i\n",eventinsert,eventextract);
        for(int i = 0; i < EVENTQSIZE; i++) {
            printf("%i;",eventq[i]);
        }
        printf("\n");
        exit(1);
    }
    eventq[eventinsert++] = event;
}

enum Event getNextEvent() {
    while (true) {
        if (eventinsert > eventextract) {
            enum Event ev = eventq[eventextract++];
            if (eventinsert == eventextract) {
                eventinsert = eventextract = 0;
            }
            return ev;
        }
        sleep_ms(100); 
    }
}

enum Event tickevent = TICK;
struct repeating_timer out;

bool timer_irqhandler(repeating_timer_t *rt) {
    insertEvent(TICK);
    return true; // to enable repeating
}
    
void setup_timer() {
    // need a 0.5 second regular "heart beat"
    if (!add_repeating_timer_ms(-500, timer_irqhandler, &tickevent, &out)) {
        printf("FAIL - unable to setup a repeating timer");
        exit(1);
    }
}

#define BUTTON_GPIOBASE 10
#define BUTTON_NUMBER 3

void gpio_irqhandler(uint gpio, uint32_t event) {
    insertEvent(gpio - BUTTON_GPIOBASE +1);
    gpio_acknowledge_irq(gpio, event);
}

void setupGPIO(uint gpio) {
    gpio_set_function(gpio,GPIO_FUNC_SIO);
    gpio_set_dir(gpio, false);
    gpio_pull_up(gpio);
    gpio_set_input_enabled(gpio, true);
    gpio_set_irq_enabled_with_callback(gpio, GPIO_IRQ_EDGE_FALL,true,gpio_irqhandler);
};

void setup_buttons() {
    for (uint i = 0; i < BUTTON_NUMBER; i++) {
        setupGPIO(BUTTON_GPIOBASE+i);
    }
}

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
enum Event event;

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
    onStart();
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
        warningFlagDown();
        timezoneTickup(0,0);
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
        timezoneTickup(mins(), secs());
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

void controllerRun() {
    onInit();
    setup_buttons();
    sleep_ms(5*1000);
    onStart();
    setup_timer();
    while (state != STOP) {
        event = getNextEvent();
        (*statetable[state][event].action)();
    }
    onExit();
}
