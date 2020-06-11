#pragma once

#include "raster/Input.h"

#include <Windows.h>
#include <stdint.h>

struct Canvas
{
    uint32_t width, height;
    uint8_t *framebuffer;

    Input input;
    Input _last_input;
    HWND  _handle;
};

Canvas
canvas_new(const char *title, uint32_t width, uint32_t height);

void
canvas_free(Canvas &self);

bool
canvas_loop(Canvas &self);

void
canvas_flush(Canvas &self);
