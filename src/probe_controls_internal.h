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

//- actual
// LOGIC ANALYSER PROBE CONTROLS
//

#include <stdlib.h>
#include "probe_controls.h"

bool parse_pinbase(struct probe_controls* controls, char* s);
bool parse_pinwidth(struct probe_controls* controls, char* s);
bool parse_frequency(struct probe_controls* controls, char* s);
bool parse_startenabled(struct probe_controls* controls, char* s);
bool parse_startpin(struct probe_controls* controls, char* s);
bool parse_starttrigger(struct probe_controls* controls, char* s);
bool parse_eventenabled(struct probe_controls* controls, char* s);
bool parse_eventpin(struct probe_controls* controls, char* s);
bool parse_eventtrigger(struct probe_controls* controls, char* s);
bool parse_sampleendmode(struct probe_controls* controls, char* s);
bool parse_samplesize(struct probe_controls* controls, char* s);
bool parse_int(char* s);
char* get_errormessage();
