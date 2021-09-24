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

static void test_gpio_probe_event_trigger_on_fall() {
    struct probe_controls controls;
    char* res = parse_control_parameters(&controls,"g-18-1-19200-0-18-0-1-18-2-1-64000"); 
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
    teardown_gpio_probe_event();
}

static void test_gpio_probe_event_trigger_on_rise() {
    struct probe_controls controls;
    char* res = parse_control_parameters(&controls,"g-18-1-19200-0-18-0-1-18-3-1-64000");
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
    teardown_gpio_probe_event();
}

static void test_gpio_probe_event_trigger_on_low() {
    struct probe_controls controls;
    char* res = parse_control_parameters(&controls,"g-18-1-19200-0-18-0-1-18-0-1-64000");
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
    teardown_gpio_probe_event();
}

static void test_gpio_probe_event_trigger_on_high() {
    struct probe_controls controls;
    char* res = parse_control_parameters(&controls,"g-18-1-19200-0-18-0-1-18-1-1-64000");
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
    teardown_gpio_probe_event();
}

static void test_gpio_probe_event_notenabled() {
    struct probe_controls controls;
    char* res = parse_control_parameters(&controls,"g-18-1-19200-0-18-0-0-18-2-1-64000"); 
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
    teardown_gpio_probe_event();
}

static uint callbackcount = 0;

static void callback() {
    callbackcount++;
}

static void test_gpio_probe_event_trigger_callback() {
    struct probe_controls controls;
    callbackcount = 0;
    char* res = parse_control_parameters(&controls,"g-18-1-19200-0-18-0-1-18-3-1-64000");
    if ( res != NULL ) {
        fail(res);
        return;
    }
    gpio_probe_event_init(&controls);
    gpio_set_trigger_listener(callback);
    uint pin = controls.ev_pin;
    gpio_set_dir(pin,true);
    gpio_put(pin, false);
    gpio_probe_event_start();
    pass_if_false_with_message("before trigger", has_event_triggered(), "event should not have triggered");
    pass_if_equal_uint("callbackcount before trigger", 0, callbackcount);
    gpio_put(pin, true);
    pass_if_true_with_message("after trigger", has_event_triggered(), "event should have triggered");
    pass_if_equal_uint("callbackcount after trigger", 1, callbackcount);
    gpio_put(pin, false);
    gpio_put(pin, true);
    pass_if_equal_uint("callbackcount after 2nd trigger", 1, callbackcount);
    teardown_gpio_probe_event();
}

// =============================================================================
//
// module API
//
// =============================================================================

void test_gpio_probe_event_init() {
    add_test("gpio_event_low", "ug", test_gpio_probe_event_trigger_on_low);
    add_test("gpio_event_high", "ug", test_gpio_probe_event_trigger_on_high);
    add_test("gpio_event_fall", "ug", test_gpio_probe_event_trigger_on_fall);
    add_test("gpio_event_rise", "ug", test_gpio_probe_event_trigger_on_rise);
    add_test("gpio_event_notenabled", "ug", test_gpio_probe_event_notenabled);
    add_test("gpio_event_callback", "ug", test_gpio_probe_event_trigger_callback);
}
