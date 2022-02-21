/**
 * Copyright 2022 Richard Linsdale (richard at theretiredprogrammer.uk).
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
#include <stdlib.h>
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include "screen.h"
#include "zonelayout.h"

void refresh(char* text) {
    clearForWrite(largeTimeZone,BLUE,font24, RED);
    screenWrite(text);
}

//
//   API
//
void timezoneBegin() {
    refresh(" -6:20");
}

void timezoneTickdown(uint16_t mins, uint16_t secs) {
    char buffer[10];
    sprintf(buffer," -%01i:%02i", mins, secs);
    refresh(buffer);
}

void timezoneTickup(uint16_t mins, uint16_t secs) {
    char buffer[10];
    sprintf(buffer,"%02i:%02i", mins, secs);
    refresh(buffer);
}

void timezoneEnd() {
    clear(largeTimeZone,BLUE);
}
