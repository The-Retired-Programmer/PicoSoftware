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

// ========================================================================
//
//  RUN LENGTH ENCODER
//
//  Encodes a binary bitstream as a string of H and L characters.  Runs of
//  the same value can be encoded as a decimal number followed by the
//  value.  There is a maximum number of digits in the number.
//
//  The string is divided into line segments of a maximum line length.
//
//  The stream can consist of a series of encoded bitstreams, each representing
//  a separate data source.  Each bitstream is headed by a line indicating 
//  a new bitstream and identifying the source.
//
// ========================================================================

#include <stdlib.h>
#include <string.h>
#include "digitalsampling.h"

#define RLELINESIZE 72
#define MAXDIGITS 6
 
uint32_t maxcount;
uint spaceformaxcount;
uint maxlinelength;
int (*eol_function)(const char *line);

char rlebuffer[RLELINESIZE];
char* insertptr;
bool logic_level;
uint32_t count;

static void _rle_init(uint maxdigits, uint _maxlinelength, int (*outputfunction)(const char *line)) {
    insertptr = rlebuffer;
    count = 0;
    spaceformaxcount = maxdigits + 2;
    maxcount = 0;
    while (maxdigits > 0 ) {
        maxcount = maxcount*10 + 9;
        maxdigits--;
    }
    maxlinelength = _maxlinelength;
    assert(maxlinelength <= RLELINESIZE);
    eol_function = outputfunction;
}

static void _rle_writetobuffer() {
    if (insertptr - rlebuffer >= maxlinelength - spaceformaxcount ) {
       *insertptr = '\0';
        (*eol_function)(rlebuffer);
        insertptr = rlebuffer;
    }
    insertptr+= (count == 1 ) ? sprintf(insertptr, "%c", logic_level?'H':'L')
            :sprintf(insertptr, "%i%c", count,logic_level?'H':'L');
}

static void _rle_insertvalue(bool logic_value) {
    if (count == 0) {
        logic_level =logic_value;
        count = 1;
    } else if (logic_value == logic_level) {
        count++;
        if (count == maxcount) {
            _rle_writetobuffer();
            count=0;
        }
    } else {
        _rle_writetobuffer();
        count = 1;
        logic_level = logic_value;
    }
}

// =============================================================================
//
// module API
//
// =============================================================================

// ========================================================================
//
//  extracting data bits from dma buffers and creating run length encoded
//  data for each sample pin
//
// ========================================================================

void create_RLE_encoded_sample(struct probe_controls* controls,
        struct sample_buffers *samplebuffers, int (*outputfunction)(const char *line)){
    _rle_init(MAXDIGITS, RLELINESIZE, outputfunction);
    for(uint pinoffset = 0; pinoffset < controls->pinwidth; pinoffset++) {
        printf("# %i\n", pinoffset+controls->pinbase);
        insertptr = rlebuffer;
        count = 0;
        for (uint bufferno = 0; bufferno < samplebuffers->number_of_buffers; bufferno++){
            for (uint wordno = 0; wordno < samplebuffers->buffer_size_words ; wordno++) {
                uint32_t wordvalue = samplebuffers->buffers[bufferno][wordno];
                for (int bitcount = usedbitsperword(controls) -controls->pinwidth + pinoffset; bitcount >=0 ; bitcount-=controls->pinwidth) {
                    uint32_t mask = 1u<<bitcount;
                    _rle_insertvalue((wordvalue&mask) > 0);
                }
            }
        }
        _rle_writetobuffer(); // flush final rle component into the buffer
        (*eol_function)(rlebuffer); // .. and output the buffer
    }
}

#ifdef TESTINGBUILD

char* get_rle_linebuffer() {
    return rlebuffer;
}

void rle_init(uint maxdigits, uint _maxlinelength, int (*outputfunction)(const char *line)) {
    _rle_init(maxdigits,_maxlinelength, outputfunction);
}

void rle_writetobuffer() {
    _rle_writetobuffer();
}

void rle_insertvalue(bool logic_value) {
    _rle_insertvalue(logic_value);
}

#endif
