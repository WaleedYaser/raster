#pragma once

#include <stdint.h>

struct Btn
{
    bool pressed;
    bool released;
    bool down;

    uint32_t pressed_count;
    uint32_t released_count;
};

struct Input
{
    int32_t mouse_x, mouse_y;
    int32_t mouse_dx, mouse_dy;
    int32_t mouse_wheel;

    static constexpr uint32_t btn_count = 6;
    union
    {
        struct
        {
            Btn mouse_left;
            Btn mouse_right;
            Btn mouse_mid;

            Btn key_space;
            Btn key_g;
            Btn key_p;
        };

        Btn btn_array[btn_count];
    };

};
