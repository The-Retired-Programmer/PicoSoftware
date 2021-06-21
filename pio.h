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
// PIO is used to read the logic pins 
//
#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"

void pio_init(uint pin_base, uint pin_count, float div);

void pio_arm(uint trigger_pin, bool trigger_level);

uint get_read_dreq();

io_ro_32 *getPIOsource();
