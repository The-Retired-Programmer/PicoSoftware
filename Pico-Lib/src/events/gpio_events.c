 #include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"

#include "gpio_events.h"
#include "debounce.pio.h"
#include "eventhandler.h"

void setup_sm(PIO pio, uint sm, uint from_pin, uint to_pin) {
    pio_sm_set_pins_with_mask(pio, sm, 0, 1u << to_pin);
    pio_sm_set_consecutive_pindirs(pio, sm, to_pin, 1, true);
    pio_gpio_init(pio, to_pin);
    gpio_pull_up(from_pin);
    pio_sm_config config = debounce_program_get_default_config(0);
    float clkdiv = 2. * 5. * 1000.;
    sm_config_set_clkdiv(&config, clkdiv);
    sm_config_set_in_pins(&config, from_pin); // for WAIT, IN
    sm_config_set_jmp_pin(&config, from_pin); // for JMP
    sm_config_set_set_pins(&config, to_pin, 1); // for SET
    pio_sm_init(pio, sm, 0, &config);
    pio_sm_set_enabled(pio, sm, true);
}

void gpio_callback(uint gpio, uint32_t event) {
    insert_gpio_event(gpio,event);
    gpio_acknowledge_irq(gpio, event);
}

bool gpio_debounce_enable_events(uint from_pins[], uint to_pins[], uint count, uint32_t eventmask) {
    if (count < 1 || count >8 ) return false;
    pio_clear_instruction_memory(pio0);
    pio_add_program_at_offset(pio0, &debounce_program, 0);
    uint num_sm_pio = count > 4 ? 4 : count;
    for (int sm = 0; sm < num_sm_pio; sm++) {
      setup_sm(pio0, sm, from_pins[sm], to_pins[sm]);  
    }
    if (count > 4 ) {
        pio_clear_instruction_memory(pio1);
        pio_add_program_at_offset(pio1, &debounce_program, 0);
        uint num_sm_pio = count - 4;
        for (int sm = 0; sm < num_sm_pio; sm++) {
            setup_sm(pio1, sm, from_pins[sm+4], to_pins[sm+4]);  
        }
    }
    return gpio_enable_events(to_pins, count, eventmask);
}

bool gpio_enable_events(uint pins[], uint count, uint32_t eventmask) {
    if (count < 1 || count >32 ) return false;
    gpio_set_irq_enabled_with_callback(pins[0],eventmask,true, gpio_callback);
    if (count > 1 )
        for (int i = 1; i< count; i++) {
            gpio_set_irq_enabled(pins[i],eventmask,true);
        }
    return true;
}