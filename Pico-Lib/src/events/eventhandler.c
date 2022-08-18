#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "eventhandler.h"

// build an array of on_event actions

typedef enum EVENTTYPE { EventGPIO, EventTimer } EventType; // expand to include timers, interprocessor, application

typedef struct PARAMGPIO {
        uint gpio;
        uint32_t events;
    } ParamGPIO;

typedef struct PARAMTIMER {
        uint32_t *timerindex;
    } ParamTimer;

typedef union EVENTPARAM {
    ParamGPIO paramGPIO;
    ParamTimer paramTimer;
} EventParam;
    
typedef struct EVENT {
    EventType type;
    EventParam param;
} Event;

#define EVENTBUFFERSIZE 0x40
#define EVENTBUFFERSIZEMASK 0x3f

Event eventqueue[EVENTBUFFERSIZE];
volatile uint insertpoint = 0; 
volatile uint removepoint = 0; 
Event extracted;

inline uint incrementpointer(uint pointer) {
    return (++pointer) & EVENTBUFFERSIZEMASK;
}

#define interruptmaskEvents SIO_IRQ_PROC0

bool bufferExtract() {
    if (insertpoint == removepoint) return false;
    irq_set_mask_enabled(interruptmaskEvents,false); // very basic lock - stop interrupts!
    extracted.type = eventqueue[removepoint].type;
    switch (extracted.type) {
    case EventGPIO:
        extracted.param.paramGPIO.gpio = eventqueue[removepoint].param.paramGPIO.gpio;
        extracted.param.paramGPIO.events = eventqueue[removepoint].param.paramGPIO.events;
        break;
    case EventTimer:
        extracted.param.paramTimer.timerindex = eventqueue[removepoint].param.paramTimer.timerindex;
        break;
    }
    removepoint = incrementpointer(removepoint);
    irq_set_mask_enabled(interruptmaskEvents,true); // unlock
    return true;
}

gpio_action_t gpioAction;
uint32_t eventmask;

timer_action_t timerAction;

void eventAction() {
    switch (extracted.type) {
    case EventGPIO:
        gpioAction(extracted.param.paramGPIO.gpio, extracted.param.paramGPIO.events);
        break;
    case EventTimer:
        timerAction(*extracted.param.paramTimer.timerindex);
        break;
    }
}

//============Public API=======================================

void onEventGPIO(gpio_action_t action, uint32_t mask) {
    gpioAction = action;
    eventmask = mask;
}

// event source 

void insert_gpio_event(uint gpio, uint32_t events) {
    if ((events & eventmask) != 0 ) {
        uint nextinsertpoint = incrementpointer(insertpoint);
        if (nextinsertpoint == removepoint) {
            printf("PANIC: Event buffer overrun/n");
            exit(EXIT_FAILURE);
        }
        eventqueue[insertpoint].type = EventGPIO;
        eventqueue[insertpoint].param.paramGPIO.gpio = gpio;
        eventqueue[insertpoint].param.paramGPIO.events = events;
        insertpoint = nextinsertpoint;
    }
}

void onEventTimer(timer_action_t action) {
    timerAction = action;
}

// event source 

void insert_timer_event(uint32_t *timerindex) {
    uint nextinsertpoint = incrementpointer(insertpoint);
    if (nextinsertpoint == removepoint) {
        printf("PANIC: Event buffer overrun/n");
        exit(EXIT_FAILURE);
    }
    eventqueue[insertpoint].type = EventTimer;
    eventqueue[insertpoint].param.paramTimer.timerindex = timerindex;
    insertpoint = nextinsertpoint;
}

void eventHandler() {
    Event event;
    while (true) {
        if (bufferExtract()) eventAction();
    }
}
