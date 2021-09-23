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
// Digital Sampling
//
//  using PIO for sampling and DMA to transfer data into
//  a circular buffer.
//
//  listening for a particular event
//

#include <stdlib.h>
#include "hardware/pio.h"
#include "digitalsampling.h"
#include "pio_digitalsampling.h"
#include "dma_digitalsampling.h"
#include "gpio_probe_event.h"

#define NUMBER_OF_BUFFERS  4
static struct sample_buffers samplebuffers;

static char* _setuptransferbuffers(struct probe_controls* controls) {
    samplebuffers.sampling_done = false;
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

static void _teardowntransferbuffers() {
    for (int i = 0; i< NUMBER_OF_BUFFERS; i++) {
        free(samplebuffers.buffers[i]);
    }
}

static enum sample_end_mode sampleendmode;

static void on_gpio_event() {
    dma_stop_where_now_is_in_window(sampleendmode - EVENT_WINDOW_1 +1);
}

// =============================================================================
//
// module API
//
// =============================================================================

char* digitalsampling_start(struct probe_controls* controls) {
    char* errormessage;
    errormessage = _setuptransferbuffers(controls);
    if (errormessage != NULL) {
        return errormessage;
    }
    piodigitalsampling_init(controls);
    errormessage = setupDMAcontrollers(controls, &pio0->rxf[0], pio_get_dreq(pio0, 0, false));
    if (errormessage != NULL) {
        return errormessage;
    }
    gpio_probe_event_init(controls);
    gpio_set_trigger_listener(on_gpio_event);
    sampleendmode = controls->sampleendmode;
    // and start everything running
    piodigitalsampling_start();
    dma_start();
    gpio_probe_event_start();
    return NULL;
}

void digitalsampling_teardown() {
    teardown_gpio_probe_event();
    teardown_dma();
    teardown_piodigitalsampling();
    _teardowntransferbuffers();
}

void digitalsampling_stop() {
    dma_stop();
}

bool is_digitalsampling_finished() {
    return samplebuffers.sampling_done;
}

struct sample_buffers *getsamplebuffers() {
    return &samplebuffers;
}

#ifdef TESTINGBUILD

char* setuptransferbuffers(struct probe_controls* controls) {
    return _setuptransferbuffers(controls);
}

void teardowntransferbuffers() {
    _teardowntransferbuffers();
}

#endif
