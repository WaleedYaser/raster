#pragma once

#include <Windows.h>
#include <stdint.h>

struct Canvas
{
    HWND handle;
    uint32_t width, height;
    uint8_t *framebuffer;
};

Canvas
canvas_new(const char *title, uint32_t width, uint32_t height);

void
canvas_free(Canvas &self);

bool
canvas_loop(Canvas &self, bool &mouse_down, int32_t &mouse_x, int32_t &mouse_y);

void
canvas_flush(Canvas &self);
