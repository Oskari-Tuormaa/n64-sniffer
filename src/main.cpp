#include <cstdio>
#include <ctime>
#include <span>

#include <hardware/structs/systick.h>
#include <pico/stdlib.h>
#include <utility>

#include "n64-protocol.pio.h"
#include "util.hpp"

#include "usb.hpp"

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

constexpr size_t out_buf_size = 32;
struct TimerCallbackData
{
    PIO&                                             pio;
    size_t                                           coords_tail { 0 };
    size_t                                           coords_head { 0 };
    std::array<std::array<uint8_t, 4>, out_buf_size> out_buf {};
};
bool timer_callback(repeating_timer_t* timer)
{
    auto& cb_data = *static_cast<TimerCallbackData*>(timer->user_data);
    PIO&  pio     = cb_data.pio;

    auto& buf           = cb_data.out_buf[cb_data.coords_head];
    cb_data.coords_head = (cb_data.coords_head + 1) % cb_data.out_buf.size();

    send_data(pio, 0x01);

    buf[0] = pio_sm_get_blocking(pio, 0);
    buf[1] = pio_sm_get_blocking(pio, 0);
    buf[2] = pio_sm_get_blocking(pio, 0);
    buf[3] = pio_sm_get_blocking(pio, 0);

    pio_sm_clear_fifos(pio, 0);
    pio_sm_exec_wait_blocking(pio, 0, pio_encode_jmp(gOffset));
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

    usb::init();

    constexpr float freq = 250'000;
    n64_protocol_program_init(pio, state_machine, offset, gpio_num, freq);

    TimerCallbackData cb_data { .pio = pio };
    repeating_timer_t timer;
    add_repeating_timer_ms(-33, timer_callback, &cb_data, &timer);

    while (true)
    {
        constexpr std::size_t delay_time = 10;
        sleep_ms(delay_time);

        usb::run_tasks();

        while (cb_data.coords_tail != cb_data.coords_head)
        {
            auto& buf = cb_data.out_buf[cb_data.coords_tail];

            int8_t coordx = buf[2];
            int8_t coordy = buf[3];

            bool lclick = (buf[0] & 0x20) != 0;
            bool rclick = (buf[0] & 0x80) != 0;

            int8_t vert = 0;
            if ((buf[1] & 0x08) != 0)
                vert += 1;
            if ((buf[1] & 0x04) != 0)
                vert -= 1;

            printf("Buttons: %02x %02x   ", buf[0], buf[1]);
            printf("Coords: %3d : %3d\r\n", coordx, coordy);
            usb::ctrl_mouse(lclick, rclick, coordx / 3, -coordy / 3, vert);

            cb_data.coords_tail
                = (cb_data.coords_tail + 1) % cb_data.out_buf.size();
        }
    }
}
