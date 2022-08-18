#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "timer_events.h"
#include "eventhandler.h"
#include "statemachine.h"

void timeraction(uint32_t timerindex);

//    --------  API  ----------

void timerActionsINIT() {
    onEventTimer(timeraction);
    if (!timer_enable_events(500)) {
        printf("PANIC: failed to setup/enable timers\n");
        exit(EXIT_FAILURE);
    }
}

//  -----------------------------------

void timeraction(uint32_t timerindex) {
    printf("Timer Event ACTION: timer %i\n", timerindex);
    statemachineAction(TICK);
}