#include <array>
#include <pico/stdlib.h>

#include "n64-protocol.pio.h"

static constexpr int gpio_num = 0;

bool timer_callback(repeating_timer_t* timer)
{
    constexpr unsigned               shift = 24U;
    constexpr std::array<uint8_t, 1> data { 0x00 };

    PIO& pio = *static_cast<PIO*>(timer->user_data);
    for (const auto& var : data)
    {
        pio_sm_put(pio, 0, var << shift);
    }
    return true;
}

int main()
{
    // NOLINTNEXTLINE(*-cstyle-cast)
    PIO  pio           = pio0;
    uint state_machine = 0;
    uint offset        = pio_add_program(pio, &n64_protocol_program);

    constexpr float freq = 250'000;
    n64_protocol_program_init(pio, state_machine, offset, gpio_num, freq);

    repeating_timer_t timer;
    add_repeating_timer_ms(-1, timer_callback, &pio, &timer);

    while (true) { }
}
