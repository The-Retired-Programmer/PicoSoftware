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

char buffer[255];
char textbuffer[255];

void wait_for_response() {
    char c;
    while ((c =uart_getc(uart1)) != '\n') {
        uart_putc(uart0,c >= ' '?c:'#');
    }
    uart_putc(uart0,'\n');
}

void wrapcommandwithbrackets(int l) {
    buffer[0] = '{';
    buffer[l+1] = '}';
    buffer[l+2] = '\0';
}

void writestring(char* string) {
    uart_puts(uart1, string);
    uart_puts(uart0, string);
}

void putCommand(char* commandtext) {
    wrapcommandwithbrackets(sprintf(buffer+1,commandtext));
    writestring(buffer);
    wait_for_response();
}

void putCommand1(char* commandtext, uint16_t p1) {
    wrapcommandwithbrackets(sprintf(buffer+1,commandtext,p1));
    writestring(buffer);
    wait_for_response();
}

void putCommand2(char* commandtext, uint16_t p1, uint16_t p2) {
    wrapcommandwithbrackets(sprintf(buffer+1,commandtext,p1,p2));
    writestring(buffer);
    wait_for_response();
}

void putCommand4(char* commandtext, uint16_t p1, uint16_t p2, uint16_t p3, uint16_t p4) {
    wrapcommandwithbrackets(sprintf(buffer+1, commandtext,p1, p2, p3, p4));
    writestring(buffer);
    wait_for_response();
}

void putCommand5(char* commandtext, uint16_t p1, uint16_t p2, uint16_t p3, uint16_t p4, uint16_t p5) {
    wrapcommandwithbrackets(sprintf(buffer+1, commandtext,p1, p2, p3, p4, p5));
    writestring(buffer);
    wait_for_response();
}

void putText(char* text) {
    wrapcommandwithbrackets(sprintf(buffer+1,"txt,%s",text));
    writestring(buffer);
    wait_for_response();
}

/* 
 *   The API
 */

void screenBegin(){
    // Initialise UART 1
    uart_init(uart1, 9600);
    // Set the GPIO pin mux to the UART - 4 is TX, 5 is RX
    gpio_set_function(4, GPIO_FUNC_UART);
    gpio_set_function(5, GPIO_FUNC_UART);
}

void clearScreen(uint16_t backgroundcolour, uint16_t fontselection) {
    putCommand1("background-colour,%i",backgroundcolour);
    putCommand("clear-screen");
    putCommand1("font,%i",fontselection);
    putCommand1("goto-pixel,0,%i",fontselection == FONT24PT?33:12);
}

void setFont(uint16_t fontselection) {
    putCommand1("font,%i",fontselection);
}

void setTextColour(uint16_t colour) {
    putCommand1("text-colour,%i",colour);
}

void drawLine(uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY, uint16_t colour){
    putCommand5("line,%i,%i,%i,%i,%i",startX,startY,endX,endY,colour);
}

void drawBox(uint16_t topleftX, uint16_t topleftY, uint16_t bottomrightX, uint16_t bottomrightY, uint16_t colour){
    putCommand5("box,%i,%i,%i,%i,%i",topleftX,topleftY,bottomrightX,bottomrightY,colour);
}

void drawFilledBox(uint16_t topleftX, uint16_t topleftY, uint16_t bottomrightX, uint16_t bottomrightY, uint16_t colour){
    putCommand5("filled-box,%i,%i,%i,%i,%i",topleftX,topleftY,bottomrightX,bottomrightY,colour);
}

void drawCircle(uint16_t centreX, uint16_t centreY, uint16_t radius, uint16_t colour){
    putCommand4("circle,%i,%i,%i,%i",centreX,centreY,radius,colour);
}

void drawFilledCircle(uint16_t centreX, uint16_t centreY, uint16_t radius, uint16_t colour) {
    putCommand4("filled-circle,%i,%i,%i,%i",centreX,centreY,radius,colour);
}

void setTextPos(uint16_t x, uint16_t y) {
    putCommand2("goto-pixel,%i,%i",x,y);
}

void screenWrite(char* text) {
    strcpy(textbuffer,text);
    char* tptr = textbuffer;
    char* startseg = textbuffer;
    while (*tptr) {
        if (*tptr == '{' ) {
            *tptr = '\0';
            putText(startseg);
            putCommand("open-bracket");
            tptr++;
            startseg = tptr;
        } else {
            tptr++;
        }
    }
    putText(startseg);
}

void screenEnd(){
    uart_deinit(uart1);
}