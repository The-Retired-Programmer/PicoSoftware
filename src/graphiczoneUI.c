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

#define clearZone drawFilledBox(0, ZONEGRAPHIC_TOP,WIDTH,ZONEGRAPHIC_BOTTOM,GREEN)

#define clearWarningFlagArea drawFilledBox(ZONELEFTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN,ZONEGRAPHIC_TOP,ZONELEFTGRAPHIC_OFFSET_HORIZONTAL+ZONELEFTGRAPHIC_WIDTH-1,ZONEGRAPHIC_BOTTOM,GREEN)
#define clearPrepFlagArea drawFilledBox(ZONERIGHTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN,ZONEGRAPHIC_TOP,ZONERIGHTGRAPHIC_OFFSET_HORIZONTAL+ZONERIGHTGRAPHIC_WIDTH-1,ZONEGRAPHIC_BOTTOM,GREEN)
#define clearWarningFlagFlashArea drawFilledBox(ZONELEFTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN,ZONEGRAPHIC_TOP+ZONEFLAGMIDDLE_TOP,ZONELEFTGRAPHIC_OFFSET_HORIZONTAL+ZONELEFTGRAPHIC_WIDTH-1,ZONEGRAPHIC_TOP+ZONEFLAGMIDDLE_BOTTOM,GREEN)
#define clearPrepFlagFlashArea drawFilledBox(ZONERIGHTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN,ZONEGRAPHIC_TOP+ZONEFLAGMIDDLE_TOP,ZONERIGHTGRAPHIC_OFFSET_HORIZONTAL+ZONERIGHTGRAPHIC_WIDTH-1,ZONEGRAPHIC_TOP+ZONEFLAGMIDDLE_BOTTOM,GREEN)
#define clearWarningUpCountdownArea drawFilledBox(ZONELEFTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN,ZONEGRAPHIC_TOP+ZONEFLAGHIGH_TOP,ZONELEFTGRAPHIC_OFFSET_HORIZONTAL+ZONELEFTGRAPHIC_WIDTH-1,ZONEGRAPHIC_TOP+ZONEFLAGHIGH_BOTTOM,GREEN)
#define clearPrepUpCountdownArea drawFilledBox(ZONERIGHTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN,ZONEGRAPHIC_TOP+ZONEFLAGHIGH_TOP,ZONERIGHTGRAPHIC_OFFSET_HORIZONTAL+ZONERIGHTGRAPHIC_WIDTH-1,ZONEGRAPHIC_TOP+ZONEFLAGHIGH_BOTTOM,GREEN)
#define clearWarningDownCountdownArea drawFilledBox(ZONELEFTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN,ZONEGRAPHIC_TOP+ZONEFLAGLOWTOP,ZONELEFTGRAPHIC_OFFSET_HORIZONTAL+ZONELEFTGRAPHIC_WIDTH-1,ZONEGRAPHIC_TOP+ZONEFLAGLOW_BOTTOM,GREEN)
#define clearPrepDownCountdownArea drawFilledBox(ZONERIGHTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN,ZONEGRAPHIC_TOP+ZONEFLAGHLOW_TOP,ZONERIGHTGRAPHIC_OFFSET_HORIZONTAL+ZONERIGHTGRAPHIC_WIDTH-1,ZONEGRAPHIC_TOP+ZONEFLAGLOW_BOTTOM,GREEN)

void drawWarningDowm() {
uint16_t topleftX = ZONELEFTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN;
    uint16_t topleftY = ZONEGRAPHIC_TOP+ZONEFLAGLOW_TOP;
    uint16_t bottomrightX = ZONELEFTGRAPHIC_OFFSET_HORIZONTAL+ZONELEFTGRAPHIC_WIDTH-1;
    uint16_t bottomrightY = ZONEGRAPHIC_TOP+ZONEFLAGLOW_BOTTOM;
    drawFilledBox(topleftX,topleftY,bottomrightX,bottomrightY,WHITE);
    drawFilledCircle(topleftX+(bottomrightX-topleftX)/3,(topleftY+bottomrightY)/2,(topleftY+bottomrightY)/4,RED);    
}

void drawWarningUp() {
    uint16_t topleftX = ZONELEFTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN;
    uint16_t topleftY = ZONEGRAPHIC_TOP+ZONEFLAGHIGH_TOP;
    uint16_t bottomrightX = ZONELEFTGRAPHIC_OFFSET_HORIZONTAL+ZONELEFTGRAPHIC_WIDTH-1;
    uint16_t bottomrightY = ZONEGRAPHIC_TOP+ZONEFLAGHIGH_BOTTOM;
    drawFilledBox(topleftX,topleftY,bottomrightX,bottomrightY,WHITE);
    drawFilledCircle(topleftX+(bottomrightX-topleftX)/3,(topleftY+bottomrightY)/2,(topleftY+bottomrightY)/4,RED);
}

void drawPrepDowm() {
    uint16_t topleftX = ZONERIGHTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN;
    uint16_t topleftY = ZONEGRAPHIC_TOP+ZONEFLAGHLOW_TOP;
    uint16_t bottomrightX = ZONERIGHTGRAPHIC_OFFSET_HORIZONTAL+ZONERIGHTGRAPHIC_WIDTH-1;
    uint16_t bottomrightY = ZONEGRAPHIC_TOP+ZONEFLAGLOW_BOTTOM;
    drawFilledBox(topleftX,topleftY,bottomrightX,bottomrightY,BLUE);
    drawFilledBox(topleftX+(bottomrightX-topleftX)/3,topleftY+(bottomrightY-topleftY)/3,
        topleftX+2*(bottomrightX-topleftX)/3,topleftY+2*(bottomrightY-topleftY)/3,WHITE);
}

void drawPrepUp() {
    uint16_t topleftX = ZONERIGHTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN;
    uint16_t topleftY = ZONEGRAPHIC_TOP+ZONEFLAGHIGH_TOP;
    uint16_t bottomrightX = ZONERIGHTGRAPHIC_OFFSET_HORIZONTAL+ZONERIGHTGRAPHIC_WIDTH-1;
    uint16_t bottomrightY = ZONEGRAPHIC_TOP+ZONEFLAGHIGH_BOTTOM;
    drawFilledBox(topleftX,topleftY,bottomrightX,bottomrightY,BLUE);
    drawFilledBox(topleftX+(bottomrightX-topleftX)/3,topleftY+(bottomrightY-topleftY)/3,
        topleftX+2*(bottomrightX-topleftX)/3,topleftY+2*(bottomrightY-topleftY)/3,WHITE);
}

void drawWarningMast() {
    drawFilledBox(
        ZONELEFTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN-POLEWIDTH,
        ZONEGRAPHIC_TOP,
        ZONELEFTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN-1.
        ZONEGRAPHIC_BOTTOM,
        BLACK);
}

void drawPrepMast() {
    drawFilledBox(
        ZONERIGHTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN-POLEWIDTH,
        ZONEGRAPHIC_TOP,
        ZONERIGHTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN-1.
        ZONEGRAPHIC_BOTTOM,
        BLACK);
}

void writeWarningUpCountdownSecs(uint16_t secs) {
    char buffer[10];
    sprintf(buffer,"%i", secs);
    setFont(FONT24PT);
    setTextColour(RED);
    setTextPos(ZONELEFTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN, ZONEGRAPHIC_TOP + ZONEFLAGHIGH_TOP + 33);
    screenWrite(text);
}

void writeWarningDownCountdownSecs(uint16_t secs) {
    char buffer[10];
    sprintf(buffer,"%i", secs);
    setFont(FONT24PT);
    setTextColour(RED);
    setTextPos(ZONELEFTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN, ZONEGRAPHIC_TOP + ZONEFLAGLOW_TOP + 33);
    screenWrite(text);
}

void writePrepUpCountdownSecs(uint16_t secs) {
    char buffer[10];
    sprintf(buffer,"%i", secs);
    setFont(FONT24PT);
    setTextColour(RED);
    setTextPos(ZONERIGHTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN, ZONEGRAPHIC_TOP + ZONEFLAGHIGH_TOP + 33);
    screenWrite(text);
}

void writePrepDownCountdownSecs(uint16_t secs) {
    char buffer[10];
    sprintf(buffer,"%i", secs);
    setFont(FONT24PT);
    setTextColour(RED);
    setTextPos(ZONERIGHTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN, ZONEGRAPHIC_TOP + ZONEFLAGLOW_TOP + 33);
    screenWrite(text);
}

void writeWarningFlagUpFlash() {
    setFont(FONT24PT);
    setTextColour(RED);
    setTextPos(ZONELEFTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN, ZONEGRAPHIC_TOP + ZONEFLAGMIDDLE_TOP + 33);
    screenWrite("^^^^^^");
}

void writeWarningFlagDownFlash() {
    setFont(FONT24PT);
    setTextColour(RED);
    setTextPos(ZONELEFTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN, ZONEGRAPHIC_TOP + ZONEFLAGMIDDLE_TOP + 33);
    screenWrite("vvvvvv");
}

void writePrepFlagUpFlash() {
    setFont(FONT24PT);
    setTextColour(RED);
    setTextPos(ZONERIGHTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN, ZONEGRAPHIC_TOP + ZONEFLAGMIDDLE_TOP + 33);
    screenWrite("^^^^^^");
}

void writePrepFlagDownFlash() {
    setFont(FONT24PT);
    setTextColour(RED);
    setTextPos(ZONERIGHTGRAPHIC_OFFSET_HORIZONTAL+POLEMARGIN, ZONEGRAPHIC_TOP + ZONEFLAGMIDDLE_TOP + 33);
    screenWrite("vvvvvv");
}
//
//   API
//
void graphiczoneBegin() {
    drawFilledBox(screen.topLeft.X, screen.topL
}

void graphiczoneInit() {
    drawWarningMast();
    drawPrepMast();
    drawWarningDown();
    drawPrepDown();
}

void warningFlagUpWarning(int16_t secs){
    clearWarningUpCountdownArea;
    writeWarningUpCountdownSecs(secs);
    writeWarningFlagUpFlash();
}

void warningFlagUpWarningFlash(){
    clearWarningFlagFlashArea;
}

void warningFlagUp(){
    clearWarningFlagArea;
    drawWarningFlagUp();
}

void warningFlagDownWarning(int16_t secs){
    clearWarningDownCountdownArea;
    writeWarningDownCountdownSecs(secs);
    writeWarningFlagDownFlash();
}

void warningFlagDownWarningFlash(){
    clearWarningFlagFlashArea;
}

void warningFlagDown(){
    clearWarningFlagArea;
    drawWarningFlagDown();
}

void prepFlagUpWarning(int16_t secs){
    clearPrepUpCountdownArea;
    writePrepUpCountdownSecs(secs);
    writePrepFlagUpFlash();
}

void prepFlagUpWarningFlash(){
    clearPrepFlagFlashArea
}

void prepFlagUp(){
    clearPrepFlagArea;
    drawPrepFlagUp();
}

void prepFlagDownWarning(int16_t secs){
    clearPrepDownCountdownArea;
    writePrepDownCountdownSecs(secs);
    writePrepFlagDownFlash();
}

void prepFlagDownWarningFlash(){
    clearPrepFlagFlashArea;
}

void prepFlagDown(){
    clearPrepFlagArea;
    drawPrepFlagDown();
}

void graphiczoneEnd() {
    clearZone;
}
