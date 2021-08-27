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
// Digital Sampling - using PIO for sampling and DMA to transfer data into
//   a circular buffer.
//

#include <stdlib.h>
#include <string.h>
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/structs/bus_ctrl.h"
#include "digitalsampling.h"
#include "digitalsampling_internal.h"
#include "pio_digitalsampling.h"
#include "dma_digitalsampling.h"

char* digitalsampling_start(struct probe_controls* controls) {
    char* returned;
    returned = setuptransferbuffers(controls);
    if (returned != NULL) {
        return returned;
    }
    piodigitalsampling_init(controls);
    returned = setupDMAcontrollers(controls, &pio0->rxf[0], pio_get_dreq(pio0, 0, false));
    if (returned != NULL) {
        return returned;
    }
    piodigitalsampling_start();
    dma_start();
    return NULL;
}

// 
//   Sample capture buffers
//
#define NUMBER_OF_BUFFERS  4
struct sample_buffers samplebuffers;

char* setuptransferbuffers(struct probe_controls* controls) {
    samplebuffers.number_of_buffers = NUMBER_OF_BUFFERS;
    uint32_t buffer_size_words = controls->samplesize / (samplesperword(controls) * NUMBER_OF_BUFFERS);
    samplebuffers.buffer_size_words = buffer_size_words;
    for (int i = 0; i< NUMBER_OF_BUFFERS; i++) {
        samplebuffers.buffers[i]=malloc(buffer_size_words * sizeof(uint32_t));
        if ( samplebuffers.buffers[i] == NULL ) {
            return "could not obtain memory for sample storage";
        }
    }
    return NULL;
}

struct sample_buffers getsamplebuffers() {
    return samplebuffers;
}
