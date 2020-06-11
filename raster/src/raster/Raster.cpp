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

void
raster_triangle(
    uint8_t *framebuffer, uint32_t width, uint32_t height,
    int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
    uint8_t r, uint8_t g, uint8_t b)
{
    int32_t min_x, min_y, mid_x, mid_y, max_x, max_y;

    if (y0 <= y1 && y0 <= y2)
    {
        min_x = x0; min_y = y0;

        if (y1 <= y2)
        {
            mid_x = x1; mid_y = y1;
            max_x = x2; max_y = y2;
        }
        else
        {
            mid_x = x2; mid_y = y2;
            max_x = x1; max_y = y1;
        }
    }
    else if (y1 <= y0 && y1 <= y2)
    {
        min_x = x1; min_y = y1;

        if (y0 <= y2)
        {
            mid_x = x0; mid_y = y0;
            max_x = x2; max_y = y2;
        }
        else
        {
            mid_x = x2; mid_y = y2;
            max_x = x0; max_y = y0;
        }
    }
    else if (y2 <= y0 && y0 <= y1)
    {
        min_x = x2; min_y = y2;

        if (y0 <= y1)
        {
            mid_x = x0; mid_y = y0;
            max_x = x1; max_y = y1;
        }
        else
        {
            mid_x = x1; mid_y = y1;
            max_x = x0; max_y = y0;
        }
    }

    float m1 = (float)(max_x - min_x) / (float)(max_y - min_y);
    float m2 = (float)(mid_x - min_x) / (float)(mid_y - min_y);
    float m3 = (float)(max_x - mid_x) / (float)(max_y - mid_y);

    float xl = min_x;
    float xr = min_x;

    if (mid_x > min_x)
    {
        for (int32_t y = min_y; y < mid_y; y++)
        {
            xl += m1;
            xr += m2;
            for (int32_t x = xl; x < xr; ++x)
            {
                int32_t index = (x + y * width) * 4;
                framebuffer[index    ] = b;
                framebuffer[index + 1] = g;
                framebuffer[index + 2] = r;
            }
        }

        for (int32_t y = mid_y; y < max_y; y++)
        {
            xl += m1;
            xr += m3;
            for (int32_t x = xl; x < xr; ++x)
            {
                int32_t index = (x + y * width) * 4;
                framebuffer[index    ] = b;
                framebuffer[index + 1] = g;
                framebuffer[index + 2] = r;
            }
        }
    }
    else
    {
        for (int32_t y = min_y; y < mid_y; y++)
        {
            xl += m2;
            xr += m1;
            for (int32_t x = xl; x < xr; ++x)
            {
                int32_t index = (x + y * width) * 4;
                framebuffer[index    ] = b;
                framebuffer[index + 1] = g;
                framebuffer[index + 2] = r;
            }
        }

        for (int32_t y = mid_y; y < max_y; y++)
        {
            xl += m3;
            xr += m1;
            for (int32_t x = xl; x < xr; ++x)
            {
                int32_t index = (x + y * width) * 4;
                framebuffer[index    ] = b;
                framebuffer[index + 1] = g;
                framebuffer[index + 2] = r;
            }
        }
    }
}
