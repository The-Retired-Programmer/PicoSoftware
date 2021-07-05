/**
 * Copyright 2021 Richard Linsdale (richard at theretiredprogrammer.uk).
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

//
// Logic Analyser Probe - top level UI
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "logic_probe.h"
#include "logic_analyser.h"

#define CMD_GO "+"
#define CMD_STOP "-"
#define CMD_HZ "H"
#define CMD_KHZ "K"
#define CMD_MHZ "M"

#define RESPONSE_COLLECTING_ARMED "Probe Collecting Armed"
#define RESPONSE_COLLECTING_DONE "Probe Collecting Done"
#define RESPONSE_FREQUENCY  "Probe Frequency set: %d %s"
#define RESPONSE_UNKNOWN "Unknown Command"

void getcommandline(char* linebuffer) {
    while (true) {
        char ch = getchar();
        if (ch == '\n' || ch == '\r') {
            *linebuffer='\0';
            return;
        } else if ( ch=='\t' || (ch>= '\32' && ch <= '\126')) {
            *linebuffer++ = ch;
        }
    }
}

int main() {
    char linebuffer[200];
    stdio_init_all();
    probe_init();
    printf("CMD - running\n");
    while ( true ) {
        //if (gets(linebuffer) == NULL) {
        //    printf("CMD - finished\n");
        //    exit(1);
        // }
        getcommandline(linebuffer);
        char* tokenbuffer = strtok(linebuffer," =,:\t");
        if (strcmp(tokenbuffer,CMD_GO)==0) {
            printresponse1(RESPONSE_COLLECTING_ARMED);
            probe_go();
        } else if (strcmp(tokenbuffer,CMD_STOP)==0) {
            printresponse1(RESPONSE_COLLECTING_DONE);
        } else if (strcmp(tokenbuffer,CMD_HZ)==0) {
            double value = atof(strtok(NULL," =,:\t"));
            probe_set_HZsamplerate(value);
            printresponse3(RESPONSE_FREQUENCY, value,"Hz");
        } else if (strcmp(tokenbuffer,CMD_KHZ)==0) {
            double value = atof(strtok(NULL," =,:\t"));
            probe_set_KHZsamplerate(value);
            printresponse3(RESPONSE_FREQUENCY, value, "KHz");
        } else if (strcmp(tokenbuffer,CMD_MHZ)==0) {
            double value = atof(strtok(NULL," =,:\t"));
            probe_set_MHZsamplerate(value);
            printresponse3(RESPONSE_FREQUENCY, value, "MHz");
        } else {
            printresponse1(RESPONSE_UNKNOWN);
        }
    }
}

void printresponse3(char* message, double value, char* units) {
    printf(message, value, units);
    printf("\n");
}

void printresponse1(char* message) {
    printf(message);
    printf("\n");
}
    
void info_response(char* statusmessage) {
    printresponse1(statusmessage);
}
