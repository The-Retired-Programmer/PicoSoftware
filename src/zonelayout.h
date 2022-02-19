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
#ifndef _ZONELAYOUT_H
#define _ZONELAYOUT_H
//
#include "screen.h";
//
// SCREEN
#define HEIGHT 160
#define WIDTH 128
#define THIRDHEIGHT (HEIGHT - 70)/3
#define POLEMARGIN 10
#define POLEWIDTH 2
#define FLAGWIDTH ZONEFLAG_WIDTH-5
#define FLAGDEPTH THIRDHEIGHT

struct Point {
    uint16_t x;
    uint16_t y;
};

struct Font {
    uint16_t font;
    uint16_t baselineoffset;
}

struct Font font24 = {FONT24PT, 35};
struct Font font9 = {FONT9PT, 14};
    
struct ZoneArea {
    struct Point topleft;
    struct Point bottomleft;
    Font fontinfo;
};

typedef struct ZoneArea Zone;

Zone screen = {{0,0},{WIDTH-1, HEIGHT-1}, null};
Zone largeTimeZone = {{0,0}, {WIDTH-1,49}, font24};
Zone statusZone = {(0,50}, {WIDTH-1,69}, font9};
Zone graphicsZone = {{0,70}, {WIDTH-1,HEIGHT-1}, null};
Zone graphicsLeftZone = {{0,70}, {WIDTH/2-1,HEIGHT-1}, null};
Zone graphicsLeftFlagZone = {{POLEMARGIN,70}, {WIDTH/2-1,HEIGHT-1}, null};
Zone grahicsLeftFlagHighZone = {{POLEMARGIN,70}, {WIDTH/2-1,70+THIRDHEIGHT-1}, font24};
Zone grahicsLeftFlagMiddleZone = {{POLEMARGIN,70+THIRDHEIGHT}, {WIDTH/2-1,70+THIRDHEIGHT*2 -1}, font24};
Zone grahicsLeftFlagLowZone = {{POLEMARGIN,70+THIRDHEIGHT*2}, {WIDTH/2-1,HEIGHT-1}, font24};
Zone graphicsRightZone = {{WIDTH/2,70}, {WIDTH-1,HEIGHT-1}, null};
Zone graphicsRightFlagZone = {{WIDTH/2+POLEMARGIN,70}, {WIDTH-1,HEIGHT-1}, null};
Zone grahicsRightFlagHighZone = {{WIDTH/2+POLEMARGIN,70}, {WIDTH-1,70+THIRDHEIGHT-1}, font24};
Zone grahicsRightFlagMiddleZone = {{WIDTH/2+POLEMARGIN,70+THIRDHEIGHT}, {WIDTH-1,70+THIRDHEIGHT*2-1}, font24};
Zone grahicsRightFlagLowZone = {{WIDTH/2+POLEMARGIN,70+THIRDHEIGHT*2}, {WIDTH-1,HEIGHT-1}, font24};

void clear(Zone zone, uint16_t colour) {
    drawFilledBox(zone.topleft.X, zone.topleft.Y, zone.bottomright.X, zone.bottomright.Y, colour);
}

void clearForWrite(Zone zone, uint16_t backgroundcolour, Font fontinfo, uint16_t textcolour) {
    clear(zone,backgroundcolour);
    setFont(fontinfo.font);
    setTextColour(textcolour);
    setTextPos(zone.topleft.X, zone.topleft.Y + fontinfo.baselineoffset);
}

#ifdef TESTINGBUILD

#endif

#endif