
typedef void(*gpio_action_t) (uint gpio, uint32_t events);
void onEventGPIO(gpio_action_t action,uint32_t mask);
void insert_gpio_event(uint gpio, uint32_t event);

typedef void(*timer_action_t) (uint32_t timerindex);
void onEventTimer(timer_action_t action);
void insert_timer_event(uint32_t *timerindex);

void eventHandler();