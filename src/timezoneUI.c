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

#define clearZone drawFilledBox(0, ZONETIME_TOP,WIDTH,ZONETIME_BOTTOM,BLUE)

void refresh(char* text) {
    clearZone;
    setFont(FONT24PT);
    setTextColour(RED);
    setTextPos(10,33);
    screenWrite(text);
}

//
//   API
//

void timezoneBegin() {
    refresh(" --:--");
}

void timezoneTick(uint32_t time) {
    char buffer[10];
    sprintf(buffer,"%i",time);
    refresh(buffer);
}

void timezoneEnd() {
    clearZone;
}
