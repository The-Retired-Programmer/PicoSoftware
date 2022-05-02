#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "gpio_events.h"
#include "eventhandler.h"
#include "statemachine.h"

void gpioaction(uint gpio, uint32_t event);

//    --------  API  ----------

void buttonActionsINIT() {
    onEventGPIO( gpioaction, GPIO_IRQ_EDGE_RISE);
    uint from_pins[] = {10,11,12};
    uint to_pins[] = {6,7,8};
    if (!gpio_debounce_enable_events(from_pins, to_pins, 3, GPIO_IRQ_EDGE_RISE )) {
        printf("PANIC: failed to setup/enable buttons\n");
        exit(EXIT_FAILURE);
    }
}

//  -----------------------------------

void gpioaction(uint gpio, uint32_t event) {
    printf("GPIO Event ACTION: pin %i, Eventtype %i\n",gpio, event);
    switch (gpio) {
    case 6:
        statemachineAction(BUTTON1);
        break;
    case 7:
        statemachineAction(BUTTON2);
        break;
    case 8:
        statemachineAction(BUTTON3);
        break;
    }
}
