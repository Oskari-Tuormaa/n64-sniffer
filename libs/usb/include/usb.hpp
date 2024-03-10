#pragma once

#include <class/hid/hid.h>
#include <class/hid/hid_device.h>
#include <device/usbd.h>

#include <cstdint>

namespace usb
{
    void init();
    void run_tasks();
    void ctrl_mouse(bool lclick, bool rclick, int8_t x, int8_t y, int8_t vert);
} // namespace usb
