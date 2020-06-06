#include "raster/Raster.h"

#include <math.h>

void
raster_clear(uint8_t *framebuffer, uint32_t width, uint32_t height, uint8_t r, uint8_t g, uint8_t b)
{
    for (uint32_t i = 0; i < width * height * 4; i += 4)
    {
        framebuffer[i    ] = b;
        framebuffer[i + 1] = g;
        framebuffer[i + 2] = r;
    }
}

void
raster_line(
    uint8_t *framebuffer, uint32_t width, uint32_t height,
    int32_t x0, int32_t y0, int32_t x1, int32_t y1,
    uint8_t r, uint8_t g, uint8_t b)
{
    // bresenham's line algorithm
    // source: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    int32_t dx = abs(x1 - x0);
    int32_t sx = x0 < x1 ? 1 : -1;
    int32_t dy = -abs(y1 - y0);
    int32_t sy = y0 < y1 ? 1 : -1;
    int32_t err = dx + dy;

    while (true)
    {
        int32_t index = (x0 + y0 * width) * 4;
        framebuffer[index    ] = b;
        framebuffer[index + 1] = g;
        framebuffer[index + 2] = r;

        if (x0 == x1 && y0 == y1)
            break;

        int32_t e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}
