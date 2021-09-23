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
// =========================================================================
//
//  LOGIC PROBE - Digital Sampling Events  using GPIO for event capture
//
// =========================================================================

#include "probe_controls.h"

void gpio_probe_event_init(struct probe_controls* controls);

void teardown_gpio_probe_event();

void gpio_set_trigger_listener(void (*listener)());

void gpio_probe_event_start();

bool has_event_triggered();
