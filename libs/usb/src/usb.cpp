#include "usb.hpp"
#include "usb_descriptors.hpp"

#include <bsp/board.h>
#include <tusb.h>

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t* buffer,
                               uint16_t reqlen)
{
    printf("tud_hid_get_report_cb\r\n");
    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const* buffer,
                           uint16_t bufsize)
{
    printf("tud_hid_set_report_cb\r\n");
}

static bool mounted { false };

void tud_mount_cb(void)
{
    printf("tud_mount_cb\r\n");
    mounted = true;
}

void tud_umount_cb(void)
{
    printf("tud_umount_cb\r\n");
    mounted = false;
}

void usb::init()
{
    printf("usb::init\r\n");
    tusb_init();
}

void usb::run_tasks()
{
    tud_task();
}

void usb::ctrl_mouse(bool lclick, bool rclick, int8_t x, int8_t y, int8_t vert)
{
    // printf("send_report\r\n");
    if (mounted)
    {
        uint8_t buttons = lclick ? MOUSE_BUTTON_LEFT : 0;
        buttons |= rclick ? MOUSE_BUTTON_RIGHT : 0;
        tud_hid_mouse_report(Report::IdMouse, buttons, x, y, vert, 0);
    }
}
