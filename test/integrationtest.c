#include <stdio.h>

#include "pico/stdlib.h"
#include "ptest.h"
#include "buttonactions.h"
#include "timeractions.h"
#include "eventhandler.h"

void integrationtest() {
    stdio_init_all();
    buttonActionsINIT();
    timerActionsINIT();
    eventHandler();
}


void integrationtest_init() {
    add_test("integrationtest", "i", integrationtest);
}