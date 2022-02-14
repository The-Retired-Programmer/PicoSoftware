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
//                    SCREEN LAYOUT DEFINITIONS
// SCREEN
#define HEIGHT 160
#define WIDTH 128
// ZONES (vertical stacked areas)
// LARGE TIME DISPLAY - ZONETIME
#define ZONETIME_HEIGHT 50
#define ZONETIME_TOP 0
#define ZONETIME_BOTTOM ZONETIME_TOP+ZONETIME_HEIGHT-1
// STATUS LINE - ZONESTATUS
#define ZONESTATUS_HEIGHT 20
#define ZONESTATUS_TOP ZONETIME_BOTTOM+1
#define ZONESTATUS_BOTTOM ZONESTATUS_TOP+ZONESTATUS_HEIGHT-1
// GRAPHIC DISPLAY - ZONEGRAPHIC - (remainder of stack)
#define ZONEGRAPHIC_TOP ZONESTATUS_BOTTOM+1
#define ZONEGRAPHIC_BOTTOM HEIGHT-1
#define ZONEGRAPHIC_HEIGHT HEIGHT-ZONEGRAPHIC_TOP
// SUBZONES - GRAPHIC spilt into two AREAS  - horizontal split (relative to parent)
// ZONELEFTGRAPHIC
#define ZONELEFTGRAPHIC_OFFSET_HORIZONTAL 0
#define ZONELEFTGRAPHIC_WIDTH WIDTH/2
// ZONERIGHTGRAPHIC
#define ZONERIGHTGRAPHIC_OFFSET_HORIZONTAL WIDTH/2
#define ZONERIGHTGRAPHIC_WIDTH WIDTH/2;
// NOW SUBZONE WITHIN LEFTGRAPHIC and RIGHTGRAPHIC FOR FLAGS (the same for both)
#define POLEMARGIN 10
#define ZONEFLAG_OFFSET_HORIZONTAL POLEMARGIN
#define ZONEFLAG_WIDTH  ZONELEFTGRAPHIC_WIDTH-POLEMARGIN
// NOW SUBZONES - within FLAG ZONE - Relative to FLAG ZONE
#define THIRDHEIGHT ZONEGRAPHIC_HEIGHT/3
// three vertically stacked zones
#define ZONEFLAGHIGH_TOP 0
#define ZONEFLAGHIGH_HEIGHT THIRDHEIGHT
#define ZONEFLAGHIGH_BOTTOM ZONEFLAGHIGH_TOP+ZONEFLAGHIGH_HEIGHT-1
#define ZONEFLAGMIDDLE_TOP ZONEFLAGHIGH_BOTTOM+1
#define ZONEFLAGMIDDLE_HEIGHT THIRDHEIGHT
#define ZONEFLAGMIDDLE_BOTTOM ZONEFLAGMIDDLE_TOP+ZONEFLAGMIDDLE_HEIGHT-1
#define ZONEFLAGLOW_TOP ZONEFLAGMIDDLE_BOTTOM+1
#define ZONEFLAGLOW_HEIGHT THIRDHEIGHT
#define ZONEFLAGLOW_BOTTOM ZONEFLAGLOW_TOP+ZONEFLAGLOW_HEIGHT-1
// dimensions for flags
#define FLAGWIDTH ZONEFLAG_WIDTH-5
#define FLAGDEPTH THIRDHEIGHT

#ifdef TESTINGBUILD

#endif

#endif