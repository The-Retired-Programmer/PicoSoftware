/**
 * Copyright 2021 Richard Linsdale (richard at theretiredprogrammer.uk).
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

//
// test - gpio probe event
//

#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "ptest.h"
#include "../src/gpio_probe_event.h"
#include "test_gpio_probe_event.h"
#include "../src/probe_controls.h"

//enum trigger_on { TRIGGER_ON_LOW, TRIGGER_ON_HIGH, TRIGGER_ON_FALL, TRIGGER_ON_RISE};

void test_gpio_probe_event_trigger_on_fall() {
    struct probe_controls controls;
    char* res = setup_controls_for_gpio(&controls,"g-16-1-19200-0-16-0-1-12-2-1-64000"); 
    if ( res != NULL ) {
        fail(res);
        return;
    }
    gpio_probe_event_init(&controls);
    uint pin = controls.ev_pin;
    gpio_set_dir(pin,true);
    gpio_put(pin, true);
    gpio_probe_event_start();
    pass_if_false_with_message("before trigger", has_event_triggered(), "event should not have triggered");
    gpio_put(pin, false);
    pass_if_true_with_message("after trigger", has_event_triggered(), "event should have triggered");
}

void test_gpio_probe_event_trigger_on_rise() {
    struct probe_controls controls;
    char* res = setup_controls_for_gpio(&controls,"g-16-1-19200-0-16-0-1-12-3-1-64000");
    if ( res != NULL ) {
        fail(res);
        return;
    }
    gpio_probe_event_init(&controls);
    uint pin = controls.ev_pin;
    gpio_set_dir(pin,true);
    gpio_put(pin, false);
    gpio_probe_event_start();
    pass_if_false_with_message("before trigger", has_event_triggered(), "event should not have triggered");
    gpio_put(pin, true);
    pass_if_true_with_message("after trigger", has_event_triggered(), "event should have triggered");
}

void test_gpio_probe_event_trigger_on_low() {
    struct probe_controls controls;
    char* res = setup_controls_for_gpio(&controls,"g-16-1-19200-0-16-0-1-12-0-1-64000");
    if ( res != NULL ) {
        fail(res);
        return;
    }
    gpio_probe_event_init(&controls);
    uint pin = controls.ev_pin;
    gpio_set_dir(pin,true);
    gpio_put(pin, false);
    gpio_probe_event_start();
    pass_if_true_with_message("is low", has_event_triggered(), "event should have triggered");
}

void test_gpio_probe_event_trigger_on_high() {
    struct probe_controls controls;
    char* res = setup_controls_for_gpio(&controls,"g-16-1-19200-0-16-0-1-12-1-1-64000");
    if ( res != NULL ) {
        fail(res);
        return;
    }
    gpio_probe_event_init(&controls);
    uint pin = controls.ev_pin;
    gpio_set_dir(pin,true);
    gpio_put(pin, true);
    gpio_probe_event_start();
    pass_if_true_with_message("is high", has_event_triggered(), "event should have triggered");
}

void test_gpio_probe_event_notenabled() {
    struct probe_controls controls;
    char* res = setup_controls_for_gpio(&controls,"g-16-1-19200-0-16-0-0-12-2-1-64000"); 
    if ( res != NULL ) {
        fail(res);
        return;
    }
    gpio_probe_event_init(&controls);
    uint pin = controls.ev_pin;
    gpio_set_dir(pin,true);
    gpio_put(pin, true);
    gpio_probe_event_start();
    pass_if_false_with_message("before trigger", has_event_triggered(), "event should not have triggered");
    gpio_put(pin, false);
    pass_if_false_with_message("after trigger", has_event_triggered(), "event should not have triggered");
}

char* setup_controls_for_gpio(struct probe_controls* controls, char * cmd) {
    char cmdbuffer[255]; 
    return parse_control_parameters(controls, strcpy(cmdbuffer,cmd));
}

void test_gpio_probe_event_init() {
    add_test("gpio_event_low", "gpiolow", test_gpio_probe_event_trigger_on_low);
    add_test("gpio_event_high", "gpiohigh", test_gpio_probe_event_trigger_on_high);
    add_test("gpio_event_fall", "gpiofall", test_gpio_probe_event_trigger_on_fall);
    add_test("gpio_event_rise", "gpiorise", test_gpio_probe_event_trigger_on_rise);
    add_test("gpio_event_notenabled", "gpionotenabled", test_gpio_probe_event_notenabled);
}
