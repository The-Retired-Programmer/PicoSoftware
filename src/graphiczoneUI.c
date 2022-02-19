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
    clearForWrite(graphicsLeftFlagHighZone, GREEN, font9, RED);
    char buffer[10];
    sprintf(buffer,"%i", secs);
    screenWrite(buffer);
}

void writeWarningDownCountdownSecs(uint16_t secs) {
    clearForWrite(graphicsLeftFlagLowZone, GREEN, font9, RED);
    char buffer[10];
    sprintf(buffer,"%i", secs);
    screenWrite(buffer);
}

void writePrepUpCountdownSecs(uint16_t secs) {
    clearForWrite(graphicsRightFlagHighZone, GREEN, font9, RED);
    char buffer[10];
    sprintf(buffer,"%i", secs);
    screenWrite(buffer);
}

void writePrepDownCountdownSecs(uint16_t secs) {
    clearForWrite(graphicsRightFlagLowZone, GREEN, font9, RED);
    char buffer[10];
    sprintf(buffer,"%i", secs);
    screenWrite(buffer);
}

void writeWarningFlagUpFlash() {
    clearForWrite(graphicsLeftFlagMiddleZone, GREEN, font9, RED);    
    screenWrite("^^");
}

void writeWarningFlagDownFlash() {
    clearForWrite(graphicsLeftFlagMiddleZone, GREEN, font9, RED); 
    screenWrite("vv");
}

void writePrepFlagUpFlash() {
    clearForWrite(graphicsRightFlagMiddleZone, GREEN, font9, RED); 
    screenWrite("^^");
}

void writePrepFlagDownFlash() {
    clearForWrite(graphicsRightFlagMiddleZone, GREEN, font9, RED); 
    screenWrite("vv");
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
    writeWarningFlagUpFlash();
}

void warningFlagUpWarningFlash(){
    clear(graphicsLeftFlagMiddleZone,GREEN);
}

void warningFlagUp(){
    clear(graphicsLeftFlagZone,GREEN);
    drawWarningFlagUp();
}

void warningFlagDownWarning(int16_t secs){
    writeWarningDownCountdownSecs(secs);
    writeWarningFlagDownFlash();
}

void warningFlagDownWarningFlash(){
    clear(graphicsLeftFlagMiddleZone,GREEN);
}

void warningFlagDown(){
    clear(graphicsLeftFlagZone,GREEN);
    drawWarningFlagDown();
}

void prepFlagUpWarning(int16_t secs){
    writePrepUpCountdownSecs(secs);
    writePrepFlagUpFlash();
}

void prepFlagUpWarningFlash(){
    clear(graphicsRightFlagMiddleZone,GREEN);
}

void prepFlagUp(){
    clear(graphicsRightFlagZone,GREEN);
    drawPrepFlagUp();
}

void prepFlagDownWarning(int16_t secs){
    writePrepDownCountdownSecs(secs);
    writePrepFlagDownFlash();
}

void prepFlagDownWarningFlash(){
    clear(graphicsRightFlagMiddleZone,GREEN);
}

void prepFlagDown(){
    clear(graphicsRightFlagZone,GREEN);
    drawPrepFlagDown();
}

void graphiczoneEnd() {
    clear(graphicsZone,GREEN);
}
