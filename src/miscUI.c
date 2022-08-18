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
#include "screen.h"
#include "timezoneUI.h"
#include "statuszoneUI.h"
#include "graphiczoneUI.h"
#include "zonelayout.h"


void drawSplashScreen() {
    screenBegin();
    clearScreen(BLUE,FONT9PT);
    // border
    drawFilledBox(0,0,128,5,RED);
    drawFilledBox(0,155,128,160,RED);
    drawFilledBox(0,0,5,160,RED);
    drawFilledBox(123,0,128,160,RED);
    // text
    setTextColour(WHITE);
    setTextPos(10,50);
    screenWrite("Race");
    setTextPos(10,80);
    screenWrite("Officer's");
    setTextPos(10,110);
    screenWrite("Friend");
}

void drawStartScreen() {
    clearScreen(BLACK,FONT24PT);
    timezoneBegin();
    statuszoneBegin(statusZone);
    graphiczoneBegin();
    statuszoneWrite("[left] start");
}

void drawEndScreen() {
    timezoneEnd();
    statuszoneEnd(statusZone);
    graphiczoneEnd();
    sleep_ms(1000);
    screenEnd();
}
