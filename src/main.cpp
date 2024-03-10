#include <cstdio>
#include <ctime>
#include <span>

#include <hardware/structs/systick.h>
#include <pico/stdlib.h>

#include "n64-protocol.pio.h"
#include "util.hpp"

static constexpr int gpio_num = 2;

static constexpr uint8_t bits1     = 0b1110;
static constexpr uint8_t bits0     = 0b1000;
static constexpr uint8_t bits_stop = 0b1000;

uint32_t gOffset = 0;

void send_data(PIO& pio, std::span<uint8_t> data)
{
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
}

void send_data(PIO& pio, uint8_t data)
{
    send_data(pio, { &data, 1 });
}

bool timer_callback(repeating_timer_t* timer)
{
    constexpr size_t                     buf_size = 4;
    static std::array<uint8_t, buf_size> buf;

    PIO& pio = *static_cast<PIO*>(timer->user_data);

    send_data(pio, 0x01);

    buf[0] = pio_sm_get_blocking(pio, 0);
    buf[1] = pio_sm_get_blocking(pio, 0);
    buf[2] = pio_sm_get_blocking(pio, 0);
    buf[3] = pio_sm_get_blocking(pio, 0);

    pio_sm_clear_fifos(pio, 0);
    pio_sm_exec_wait_blocking(pio, 0, pio_encode_jmp(gOffset));

    int8_t coord_x = buf[2];
    int8_t coord_y = buf[3];

    for (auto& bit : std::span { buf.data(), 2 })
    {
        for (size_t i = 0; i < 8; i++)
        {
            printf("%d", ((bit << i) & 0x80) != 0);
        }
    }
    printf("   %3d : %3d\r\n", coord_x, coord_y);
    return true;
}

int main()
{
    // NOLINTNEXTLINE(*-cstyle-cast)
    PIO  pio           = pio0;
    uint state_machine = 0;
    uint offset        = pio_add_program(pio, &n64_protocol_program);
    gOffset            = offset;

    stdio_init_all();
    printf("Hello, world!\r\n");

    constexpr float freq = 250'000;
    n64_protocol_program_init(pio, state_machine, offset, gpio_num, freq);

    repeating_timer_t timer;
    add_repeating_timer_ms(-33, timer_callback, &pio, &timer);

    while (true)
    {
        constexpr std::size_t delay_time = 1000;
        sleep_ms(delay_time);
    }
}
