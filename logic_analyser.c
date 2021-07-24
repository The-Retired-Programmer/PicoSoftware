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

void getcommandline(char* ptr) {
    while (true) {
        char ch = getchar();
        if (ch == '\n' || ch == '\r') {
            *ptr='\0';
            return;
        } else if (ch>= '\x20' && ch <= '\x7e') {
            *ptr++ = ch;
        }
    }
}

int main() {
    char linebuffer[200];
    stdio_init_all();
    probe_init();
    while ( true ) {
        getcommandline(linebuffer);
        if (strchr(linebuffer,'!') == NULL) {
            switch (linebuffer[0]) {
            case 'p': 
                probe_ping();
                break;
            case '?': 
                probe_getstate();
                break;
            case 'g': 
                probe_go(linebuffer);
                break;
            case 's': 
                probe_stop();
                break;
            case 'd':
                probe_getsample();
                break;
            default: // unknown command
                printf("N Unknown command %s\n",linebuffer);
            }
        }
    }
}
