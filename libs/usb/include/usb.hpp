#pragma once

#include <array>
#include <class/hid/hid.h>
#include <class/hid/hid_device.h>
#include <device/usbd.h>

#include <cstdint>

namespace usb
{
    void init();
    void run_tasks();
    void ctrl_mouse(bool lclick, bool rclick, int8_t mov_x, int8_t mov_y,
                    int8_t vert);
    void send_keystroke(std::array<uint8_t, 6> keys);
} // namespace usb
