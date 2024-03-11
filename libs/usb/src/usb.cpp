#include "usb.hpp"
#include "usb_descriptors.hpp"

#include <bsp/board.h>
#include <tusb.h>

#include <variant>

template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

uint16_t tud_hid_get_report_cb(uint8_t /*instance*/, uint8_t /*report_id*/,
                               hid_report_type_t /*report_type*/,
                               uint8_t* /*buffer*/, uint16_t /*reqlen*/)
{
    printf("tud_hid_get_report_cb\r\n");
    return 0;
}

void tud_hid_set_report_cb(uint8_t /*instance*/, uint8_t /*report_id*/,
                           hid_report_type_t /*report_type*/,
                           uint8_t const* /*buffer*/, uint16_t /*bufsize*/)
{
    printf("tud_hid_set_report_cb\r\n");
}

namespace
{
    bool mounted { false };

    struct MouseEvent
    {
        bool   left_click;
        bool   right_click;
        int8_t mov_x;
        int8_t mov_y;
        int8_t scroll;
    };
    struct KeyboardEvent
    {
        std::array<uint8_t, 6> keys;
    };
    using Event = std::variant<MouseEvent, KeyboardEvent>;

    struct EventItem
    {
        Event evt;
        bool  sent { false };
    };

    std::array<EventItem, 32> event_queue {};
    size_t                    event_queue_head { 0 };
    size_t                    event_queue_tail { 0 };

    bool is_event_queue_full()
    {
        return ((event_queue_head + 1) % event_queue.size())
            == event_queue_tail;
    }

} // namespace

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

void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report,
                                uint16_t len)
{
    // printf("Report complete!\r\n");
    event_queue_tail = (event_queue_tail + 1) % event_queue.size();
}

void usb::init()
{
    printf("usb::init\r\n");
    tusb_init();
}

void usb::run_tasks()
{
    tud_task();

    EventItem& current_evt = event_queue[event_queue_tail];
    if (!current_evt.sent && (event_queue_tail != event_queue_head) && mounted)
    {
        std::visit(
            overloaded {
                [](MouseEvent evt) {
                    uint8_t buttons = evt.left_click ? MOUSE_BUTTON_LEFT : 0;
                    buttons |= evt.right_click ? MOUSE_BUTTON_RIGHT : 0;
                    // printf("Sending mouse report!\r\n");
                    tud_hid_mouse_report(Report::IdMouse, buttons, evt.mov_x,
                                         evt.mov_y, evt.scroll, 0);
                },
                [](KeyboardEvent evt) {
                    // printf("Sending keyboard report!\r\n");
                    tud_hid_keyboard_report(Report::IdKeyboard, 0,
                                            evt.keys.data());
                },
            },
            current_evt.evt);
        current_evt.sent = true;
    }

    hid_gamepad_report_t report;
}

void usb::ctrl_mouse(bool lclick, bool rclick, int8_t mov_x, int8_t mov_y,
                     int8_t vert)
{
    if (!mounted || is_event_queue_full())
    {
        return;
    }

    event_queue[event_queue_head] = EventItem { MouseEvent {
        .left_click  = lclick,
        .right_click = rclick,
        .mov_x       = mov_x,
        .mov_y       = mov_y,
        .scroll      = vert,
    } };

    event_queue_head = (event_queue_head + 1) % event_queue.size();

    // uint8_t buttons = lclick ? MOUSE_BUTTON_LEFT : 0;
    // buttons |= rclick ? MOUSE_BUTTON_RIGHT : 0;
    // tud_hid_mouse_report(Report::IdMouse, buttons, mov_x, mov_y, vert, 0);
}

void usb::send_keystroke(std::array<uint8_t, 6> keys)
{
    if (!mounted || is_event_queue_full())
    {
        return;
    }

    event_queue[event_queue_head] = EventItem { KeyboardEvent {
        .keys = keys,
    } };

    event_queue_head = (event_queue_head + 1) % event_queue.size();

    // tud_hid_keyboard_report(Report::IdKeyboard, 0, keys.data());
}
