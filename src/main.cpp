#include <span>

#include <pico/stdlib.h>

#include "n64-protocol.pio.h"
#include "util.hpp"

static constexpr int gpio_num = 0;

static constexpr uint8_t bits1     = 0b1110;
static constexpr uint8_t bits0     = 0b1000;
static constexpr uint8_t bits_stop = 0b1000;

void send_data(PIO& pio, std::span<uint8_t> data)
{
    uint mask = 1U << gpio_num;
    pio_sm_set_pindirs_with_mask(pio, 0, 0xff, mask);
    for (auto val : data)
    {
        for (size_t i = 0; i < 8; i++, val <<= 1)
        {
            if ((val & 0x80) == 0)
            {
                pio_sm_put_blocking(pio, 0, bits0);
            }
            else
            {
                pio_sm_put_blocking(pio, 0, bits1);
            }
        }
    }
    pio_sm_put_blocking(pio, 0, bits_stop);
    while (!pio_sm_is_tx_fifo_empty(pio, 0)) { }
    nop<200>();
    pio_sm_set_pindirs_with_mask(pio, 0, 0x00, mask);
}

void send_data(PIO& pio, uint8_t data)
{
    send_data(pio, { &data, 1 });
}

bool timer_callback(repeating_timer_t* timer)
{
    PIO& pio = *static_cast<PIO*>(timer->user_data);
    send_data(pio, 0x00);
    return true;
}

int main()
{
    // NOLINTNEXTLINE(*-cstyle-cast)
    PIO  pio           = pio0;
    uint state_machine = 0;
    uint offset        = pio_add_program(pio, &n64_protocol_program);

    stdio_init_all();

    constexpr float freq = 1'000'000;
    gpio_init(gpio_num);
    gpio_pull_up(gpio_num);
    n64_protocol_program_init(pio, state_machine, offset, gpio_num, freq);

    repeating_timer_t timer;
    add_repeating_timer_ms(-1, timer_callback, &pio, &timer);

    while (true)
    {
        constexpr std::size_t delay_time = 1000;
        sleep_ms(delay_time);
    }
}
