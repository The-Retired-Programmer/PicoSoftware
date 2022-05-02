#include <stdio.h>
#include "pico/stdlib.h"

#include "eventhandler.h"

struct repeating_timer out;

uint timerindex = 0;

bool timer_callback(repeating_timer_t *rt) {
    insert_timer_event(rt->user_data);
    return true;
}

bool timer_enable_events(int32_t delay_ms) {
    return add_repeating_timer_ms(-delay_ms, timer_callback, &timerindex, &out);
}