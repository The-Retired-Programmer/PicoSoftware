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

#include <stdlib.h>
#include "screen.h"

#define HEIGHT 160
#define WIDTH 128

struct Point {
    uint16_t x;
    uint16_t y;
};

typedef struct {
    uint16_t font;
    uint16_t baselineoffset;
} Font;

#define font24 (Font){FONT24PT,35}
#define font9 (Font){FONT9PT,14}
    
typedef struct {
    struct Point topleft;
    struct Point bottomright;
} Zone;

#define screen (Zone){{0,0},{WIDTH-1,HEIGHT-1}}

//layouts for pre start screen

#define THIRDHEIGHT (HEIGHT - 70)/3
#define POLEMARGIN 10
#define POLEWIDTH 2
#define FLAGWIDTH ZONEFLAG_WIDTH-5
#define FLAGDEPTH THIRDHEIGHT

#define largeTimeZone (Zone){{0,0},{WIDTH-1,49}}
#define statusZone (Zone){{0,50},{WIDTH-1,69}}
#define graphicsZone (Zone){{0,70},{WIDTH-1,HEIGHT-1}}
#define graphicsLeftZone (Zone){{0,70},{WIDTH/2-1,HEIGHT-1}}
#define graphicsLeftMastZone (Zone){{POLEMARGIN-POLEWIDTH,70},{POLEMARGIN-1,HEIGHT-1}}
#define graphicsLeftFlagZone (Zone){{POLEMARGIN,70},{WIDTH/2-1,HEIGHT-1}}
#define graphicsLeftFlagHighZone (Zone){{POLEMARGIN,70},{WIDTH/2-1,70+THIRDHEIGHT-1}}
#define graphicsLeftFlagHighMiddleZone (Zone){{POLEMARGIN,70},{WIDTH/2-1,70+THIRDHEIGHT*2 -1}}
#define graphicsLeftFlagMiddleLowZone (Zone){{POLEMARGIN,70+THIRDHEIGHT},{WIDTH/2-1,HEIGHT-1}}
#define graphicsLeftFlagLowZone (Zone){{POLEMARGIN,70+THIRDHEIGHT*2},{WIDTH/2-1,HEIGHT-1}}
#define graphicsRightZone (Zone){{WIDTH/2,70},{WIDTH-1,HEIGHT-1}}
#define graphicsRightMastZone (Zone){{WIDTH/2+POLEMARGIN-POLEWIDTH,70},{WIDTH/2+POLEMARGIN-1,HEIGHT-1}}
#define graphicsRightFlagZone (Zone){{WIDTH/2+POLEMARGIN,70}, {WIDTH-1,HEIGHT-1}}
#define graphicsRightFlagHighZone (Zone){{WIDTH/2+POLEMARGIN,70},{WIDTH-1,70+THIRDHEIGHT-1}}
#define graphicsRightFlagHighMiddleZone (Zone){{WIDTH/2+POLEMARGIN,70},{WIDTH-1,70+THIRDHEIGHT*2-1}}
#define graphicsRightFlagMiddleLowZone (Zone){{WIDTH/2+POLEMARGIN,70+THIRDHEIGHT},{WIDTH-1,HEIGHT-1}}
#define graphicsRightFlagLowZone (Zone){{WIDTH/2+POLEMARGIN,70+THIRDHEIGHT*2},{WIDTH-1,HEIGHT-1}}

// layout for post start screen

#define largeMinsTimeZone (Zone){{0,0},{WIDTH-1,49}}
#define largeSecsTimeZone (Zone){{0,50},{WIDTH-1,99}}

#define racingStatusZone (Zone){{0,HEIGHT-20},{WIDTH-1,HEIGHT-1}}

void clear(Zone zone, uint16_t colour);
void clearForWrite(Zone zone, uint16_t backgroundcolour, Font fontinfo, uint16_t textcolour);
void paint(Zone zone, uint16_t colour);
void paintinsert(Zone zone, uint16_t colour);
void paintinsertcircle(Zone zone, uint16_t colour);

#ifdef TESTINGBUILD

#endif

#endif