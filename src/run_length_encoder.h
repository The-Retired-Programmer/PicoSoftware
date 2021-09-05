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

#include "probe_controls.h"
#include "digitalsampling.h"

void create_RLE_encoded_sample(struct probe_controls* probecontrols,
    struct sample_buffers samplebuffers, int (*outputfunction)(const char *line));

#ifdef TESTINGBUILD

char* get_rle_linebuffer();

void rle_init(uint maxdigits, uint _maxlinelength, int (*_outputfunction)(const char *line));

void rle_writetobuffer();

void rle_insertvalue(bool logic_value);

#endif