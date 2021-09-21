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
// Logic Analyser Probe
//

#include <stdlib.h>
#include "pico/stdlib.h"
#include <stdio.h>
#include "logic_probe.h"
#include "frontend_commands.h"

static int ack_puts() {
    puts("Y");
}

static int nak_puts(char *response) {
    printf("N %s\n", response);
}

// =============================================================================
//
// module API  - the main function
//
// =============================================================================

int main() {
    stdio_init_all();
    probe_init(puts,ack_puts, nak_puts);
    frontend_commands_controller_init();
    while (true) {
        frontend_commands_controller();
        is_probe_stop_complete();
    }
}
