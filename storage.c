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
// storage manager
//

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "dma.h"

// buffer(s) to hold the captured data 

uint32_t *capture_buf;
uint buf_size_words;

void storage_init(uint samplesize, uint pinspersample) {
    buf_size_words = (samplesize * pinspersample + 31) / 32;
    capture_buf = malloc(buf_size_words * sizeof(uint32_t));
    hard_assert(capture_buf);
    dma_init();
}

void storage_arm() {
    dma_arm(capture_buf, buf_size_words);
}

void storage_waituntilcompleted(){
    dma_waituntilcompleted();
}

uint32_t *get_capturebuf() {
    return capture_buf;
}

uint get_capturebuf_size() {
    return buf_size_words;
}
