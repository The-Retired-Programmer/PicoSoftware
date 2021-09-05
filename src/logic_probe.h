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

void probe_init();

void probe_pass_init();
void probe_pass_teardown();

void probe_ping();
void probe_getstate();
void probe_go(char* cmdbuffer);
void probe_stop();
bool is_probe_stop_complete();
void probe_getsample();

#ifdef TESTINGBUILD

#include "probe_controls.h"

enum probestate getprobestate();

#endif
