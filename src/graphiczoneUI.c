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

#define clearZone drawFilledBox(0, ZONEGRAPHIC_TOP,WIDTH,ZONEGRAPHIC_BOTTOM,WHITE)

void graphiczoneBegin() {
    clearZone;
}

void uiRaiseWarning() {
    drawFilledBox(0,60,60,155,BLACK);
    drawLine(0,155,0,60,WHITE);
    drawFilledBox(0,65,60,85,WHITE);
    drawFilledCircle(30,75,7,RED);
}

void uiRaisePreparatory() {
    drawFilledBox(60,60,120,155,BLACK);
    drawLine(60,155,60,60,WHITE);
    drawFilledBox(60,65,120,95,BLUE);
    drawFilledBox(75,72,105,87,WHITE);
}

void graphiczoneTick() {
}

void graphiczoneEnd() {
    clearZone;
}
