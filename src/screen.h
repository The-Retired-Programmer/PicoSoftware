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

void clearScreen(uint backgroundcolour, uint fontselection);

void setFont(uint fontselection);

void setTextColour(uint colour);

void drawLine(uint startx, uint startY, uint endX, uint endY, uint colour);

void drawBox(uint topleftX, uint topleftY, uint bottomrightX, uint bottomrightY, uint colour);

void drawFilledBox(uint topleftX, uint topleftY, uint bottomrightX, uint bottomrightY, uint colour);

void drawCircle(uint centreX, uint centreY, uint radius, uint colour);

void drawFilledCircle(uint centreX, uint centreY, uint radius, uint colour);

void setTextPos(uint x, uint y);

void screenWrite(char* text);

void screenEnd();

#ifdef TESTINGBUILD

#endif

#endif