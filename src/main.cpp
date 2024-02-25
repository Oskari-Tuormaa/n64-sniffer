#include <pico/stdlib.h>

#include "squarewave.pio.h"

static constexpr int gpio_num = 0;

bool timer_callback(repeating_timer_t* t)
{
    PIO& pio = *static_cast<PIO*>(t->user_data);
    pio_sm_put(pio, 0, 0x01 << 24U);
    pio_sm_put(pio, 0, 0x12 << 24U);
    return true;
}

int main()
{
    PIO  pio    = pio0;
    uint sm     = 0;
    uint offset = pio_add_program(pio, &squarewave_program);

    pio_gpio_init(pio, gpio_num);
    pio_sm_set_consecutive_pindirs(pio, sm, gpio_num, 1, true);

    pio_sm_config c = squarewave_program_get_default_config(offset);
    sm_config_set_sideset_pins(&c, gpio_num);
    sm_config_set_out_shift(&c, false, true, 8);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    sm_config_set_clkdiv(&c, 65.5F);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);

    repeating_timer_t timer;
    add_repeating_timer_ms(-250, timer_callback, &pio, &timer);

    while (true) { }
}
