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

#include "zonelayout.h"

void clear(Zone zone, uint16_t colour) {
    drawFilledBox(zone.topleft.x, zone.topleft.y, zone.bottomright.x, zone.bottomright.y, colour);
}

void clearForWrite(Zone zone, uint16_t backgroundcolour, Font fontinfo, uint16_t textcolour) {
    clear(zone,backgroundcolour);
    setFont(fontinfo.font);
    setTextColour(textcolour);
    setTextPos(zone.topleft.x, zone.topleft.y + fontinfo.baselineoffset);
}

void paint(Zone zone, uint16_t colour) {
    clear(zone, colour);
}

void paintinsert(Zone zone, uint16_t colour) {
    drawFilledBox(zone.topleft.x+(zone.bottomright.x-zone.topleft.x)/3,zone.topleft.y+(zone.bottomright.y-zone.topleft.y)/3,
        zone.topleft.x+2*(zone.bottomright.x-zone.topleft.x)/3,zone.topleft.y+2*(zone.bottomright.y-zone.topleft.y)/3,colour);
}

void paintinsertcircle(Zone zone, uint16_t colour) {
    drawFilledCircle(zone.topleft.x+(zone.bottomright.x-zone.topleft.x)/3,(zone.topleft.y+zone.bottomright.y)/2,(zone.bottomright.y-zone.topleft.y)/4, colour);
}