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

uint32_t tickcounter; //0.5sec time counter

void onInit() {
    printf("Race Officer's friend - running\n");
    screenBegin();
    tickcounter = 0;
    drawSplashScreen();
}

void onStart() {
    clearScreen(BLACK,FONT24PT);
    timezoneBegin();
    statuszoneBegin();
    graphiczoneBegin();
    statuszoneWrite("[left] to start");
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
enum State {INIT, STOP, COUNTDOWN, COUNTUP};
typedef struct action {
    void (*action)();
} SM_action;

void sm_ignore();
void sm_panic(); 
void sm_init_b1();
void sm_init_b2();
void sm_countdown_b2();
void sm_countup_b3();
void sm_ignoretick();
void sm_tickdown();
void sm_tickup();

SM_action statetable[4][4] = {
    // INIT
    { sm_ignoretick,sm_init_b1,sm_init_b2,sm_ignore},
    // STOP
    { sm_panic,sm_panic,sm_panic,sm_panic},
    // COUNTDOWN
    { sm_tickdown,sm_ignore,sm_countdown_b2,sm_ignore},
    // COUNTUP
    { sm_tickup,sm_ignore,sm_ignore,sm_countup_b3}
};

/* ----------------------------------------------------------
 *   API 
 */
enum State state = INIT;
enum Event event;

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

//  state machine actions

void sm_ignore() {}

void sm_panic() {
    printf("STATE MACHINE PANIC - state = %i; event = %i; STOP", state,event);
    state = STOP;
}

void sm_init_b1(){ state = COUNTDOWN;}

void sm_init_b2(){ state = STOP;}

void sm_countdown_b2(){ state = COUNTUP;}

void sm_countup_b3() {state = INIT;}

void sm_ignoretick() {}

void sm_tickdown() {printf("tickdown\n");}

void sm_tickup(){ printf("tickup\n");}
    
