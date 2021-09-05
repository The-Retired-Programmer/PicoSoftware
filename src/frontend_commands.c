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
// Logic Analyser -  frontend command handling
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "logic_probe.h"

char linebuffer[200];
char *insertchar;

static void _action_command(char *line) {
    if (strchr(line,'!') == NULL) {
        switch (line[0]) {
        case 'p': 
            probe_ping();
            break;
        case '?': 
            probe_getstate();
            break;
        case 'g':
            probe_pass_init();
            probe_go(line);
            break;
        case 's': 
            probe_stop();
            break;
        case 'd':
            probe_getsample();
            probe_pass_teardown();
            break;
        default: // unknown command
            printf("N Unknown command %s\n",line);
        }
    }
}

static bool linebuilder() {
    while (true) {
        int rawch = getchar_timeout_us(0);
        if (rawch ==  PICO_ERROR_TIMEOUT) {
            return false;
        }
        char ch = (char) rawch;
        if (ch == '\n' || ch == '\r') {
            *insertchar='\0';
            return true;
        } else if (ch>= '\x20' && ch <= '\x7e') {
            *insertchar++ = ch;
        }
    }
}

// =============================================================================
//
// module API
//
// =============================================================================

void frontend_commands_controller_init() {
    insertchar = linebuffer;
}

void frontend_commands_controller() {
    if (linebuilder()) {
        _action_command(linebuffer);
        frontend_commands_controller_init();
    }
}

#ifdef TESTINGBUILD

void action_command(char *line) {
    _action_command(line);
}

#endif

