#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "gpio_events.h"
#include "eventhandler.h"
#include "timeractions.h" // temp

void gpioaction(uint gpio, uint32_t event);

//    --------  API  ----------

void buttonActionsINIT() {
    onEventGPIO( gpioaction, GPIO_IRQ_EDGE_RISE);
    uint from_pins[] = {10,11,12};
    uint to_pins[] = {6,7,8};
    if (!gpio_debounce_enable_events(from_pins, to_pins, 3, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL)) {
        printf("PANIC: failed to setup/enable buttons\n");
        exit(EXIT_FAILURE);
    }
}

//  -----------------------------------

void gpioaction(uint gpio, uint32_t event) {
    printf("GPIO Event ACTION: pin %i, Eventtype %i\n",gpio, event);
}