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
#ifndef _SCREEN_H
#define _SCREEN_H

#include <stdlib.h>
#include "pico/stdlib.h"

#define FONTTOMTHUMB 1
#define FONTPICOPIXEL 2
#define FONT9PT 3
#define FONT24PT 4
#define BLACK 0
#define BLUE 1
#define RED 2
#define GREEN 3
#define CYAN 4
#define MAGENTA 5
#define YELLOW 6
#define WHITE 7

void screenBegin();

void clearScreen(uint16_t backgroundcolour, uint16_t fontselection);

void setFont(uint16_t fontselection);

void setTextColour(uint16_t colour);

void drawLine(uint16_t startx, uint16_t startY, uint16_t endX, uint16_t endY, uint16_t colour);

void drawBox(uint16_t topleftX, uint16_t topleftY, uint16_t bottomrightX, uint16_t bottomrightY, uint16_t colour);

void drawFilledBox(uint16_t topleftX, uint16_t topleftY, uint16_t bottomrightX, uint16_t bottomrightY, uint16_t colour);

void drawCircle(uint16_t centreX, uint16_t centreY, uint16_t radius, uint16_t colour);

void drawFilledCircle(uint16_t centreX, uint16_t centreY, uint16_t radius, uint16_t colour);

void setTextPos(uint16_t x, uint16_t y);

void screenWrite(char* text);

void screenEnd();

#ifdef TESTINGBUILD

#endif

#endif