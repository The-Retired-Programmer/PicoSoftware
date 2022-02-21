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

void drawWarningFlagDown() {
    paint(graphicsLeftFlagLowZone,WHITE);
    paintinsertcircle(graphicsLeftFlagLowZone,RED);
}

void drawWarningFlagUp() {
    paint(graphicsLeftFlagHighZone,WHITE);
    paintinsertcircle(graphicsLeftFlagHighZone,RED);
}

void drawPrepFlagDown() {
    paint(graphicsRightFlagLowZone,BLUE);
    paintinsert(graphicsRightFlagLowZone,WHITE);
}

void drawPrepFlagUp() {
    paint(graphicsRightFlagHighZone,BLUE);
    paintinsert(graphicsRightFlagHighZone,WHITE);
}

void drawWarningMast() {
    paint(graphicsLeftMastZone,BLACK);
}

void drawPrepMast() {
    paint(graphicsRightMastZone,BLACK);
}

void writeWarningUpCountdownSecs(uint16_t secs) {
    clearForWrite(graphicsLeftFlagHighMiddleZone, GREEN, font24, RED);
    char buffer[10];
    sprintf(buffer,"%02i", secs);
    screenWrite(buffer);
}

void writeWarningDownCountdownSecs(uint16_t secs) {
    clearForWrite(graphicsLeftFlagMiddleLowZone, GREEN, font24, RED);
    char buffer[10];
    sprintf(buffer,"%02i", secs);
    screenWrite(buffer);
}

void writePrepUpCountdownSecs(uint16_t secs) {
    clearForWrite(graphicsRightFlagHighMiddleZone, GREEN, font24, RED);
    char buffer[10];
    sprintf(buffer,"%02i", secs);
    screenWrite(buffer);
}

void writePrepDownCountdownSecs(uint16_t secs) {
    clearForWrite(graphicsRightFlagMiddleLowZone, GREEN, font24, RED);
    char buffer[10];
    sprintf(buffer,"%02i", secs);
    screenWrite(buffer);
}
//
//   API
//
void graphiczoneBegin() {
    clear(graphicsZone,GREEN);
}

void graphiczoneInit() {
    drawWarningMast();
    drawPrepMast();
    drawWarningFlagDown();
    drawPrepFlagDown();
}

void warningFlagUpWarning(int16_t secs){
    writeWarningUpCountdownSecs(secs);
}

void warningFlagUp(){
    clear(graphicsLeftFlagZone,GREEN);
    drawWarningFlagUp();
}

void warningFlagDownWarning(int16_t secs){
    writeWarningDownCountdownSecs(secs);
}

void warningFlagDown(){
    clear(graphicsLeftFlagZone,GREEN);
    drawWarningFlagDown();
}

void prepFlagUpWarning(int16_t secs){
    writePrepUpCountdownSecs(secs);
}

void prepFlagUp(){
    clear(graphicsRightFlagZone,GREEN);
    drawPrepFlagUp();
}

void prepFlagDownWarning(int16_t secs){
    writePrepDownCountdownSecs(secs);
}

void prepFlagDown(){
    clear(graphicsRightFlagZone,GREEN);
    drawPrepFlagDown();
}

void graphiczoneEnd() {
    clear(graphicsZone,GREEN);
}
