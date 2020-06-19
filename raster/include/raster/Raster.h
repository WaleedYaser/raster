#pragma once

#include <stdint.h>

void
raster_clear(uint8_t *framebuffer, uint32_t width, uint32_t height, uint8_t r, uint8_t g, uint8_t b);

void
raster_text(
    uint8_t *framebuffer, uint32_t width, uint32_t height,
    int32_t x, int32_t y,
    uint8_t *atlas, const char *text);

void
raster_hline(
    uint8_t *framebuffer, uint32_t width, uint32_t height,
    int32_t y, int32_t x0, int32_t x1,
    uint8_t r, uint8_t g, uint8_t b);

void
raster_vline(
    uint8_t *framebuffer, uint32_t width, uint32_t height,
    int32_t x, int32_t y0, int32_t y1,
    uint8_t r, uint8_t g, uint8_t b);

void
raster_grid(uint8_t *framebuffer, uint32_t width, uint32_t height);

void
raster_line(
    uint8_t *framebuffer, uint32_t width, uint32_t height,
    int32_t x0, int32_t y0, int32_t x1, int32_t y1,
    uint8_t r, uint8_t g, uint8_t b);

void
raster_triangle(
    uint8_t *framebuffer, uint32_t width, uint32_t height,
    int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
    uint8_t r, uint8_t g, uint8_t b);
