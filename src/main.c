#include <stdio.h>

#include "pico/stdlib.h"
#include "buttonactions.h"
#include "timeractions.h"
#include "eventhandler.h"
#include "miscUI.h"

int main() {
    stdio_init_all();
    //controllerRun();
    //onInit();
    printf("Race Officer's friend - running\n");
    drawSplashScreen();
    //setup_buttons();
    buttonActionsINIT();
    sleep_ms(5*1000);
    //onStart();
    drawStartScreen();
    //setup_timer();
    timerActionsINIT();
    /*while (state != STOP) {
        event = getNextEvent();
        (*statetable[state][event].action)();
    } */
    eventHandler();
    //onExit();
    drawEndScreen();
    printf("Race Officer's friend - shut down\n");
}
