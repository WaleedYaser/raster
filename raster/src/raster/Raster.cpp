#include "raster/Raster.h"

#include <math.h>
#include <assert.h>

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
raster_text(
    uint8_t *framebuffer, uint32_t width, uint32_t height,
    int32_t x, int32_t y,
    uint8_t *atlas, const char *text)
{
    int x_init = x;

    while (uint8_t c = *text++)
    {
        if (c == '\n')
        {
            y += 20;
            x = x_init;
            continue;
        }
        if (c == ' ')
        {
            x += 9;
            continue;
        }

        uint8_t index = c - ' ';
        int32_t x_min = (index % 16) * 16;
        int32_t y_min = (index / 16) * 32;

        for (int j = 0; j < 20; ++j)
        {
            for (int i = 0; i < 9; ++i)
            {
                int32_t dst_index = ((x + i) + ((y + j) * width)) * 4;
                int32_t src_index = ((x_min + i) + ((y_min + j) * 256)) * 4;

                if (atlas[src_index])
                {
                    framebuffer[dst_index    ] = atlas[src_index];
                    framebuffer[dst_index + 1] = atlas[src_index + 1];
                    framebuffer[dst_index + 2] = atlas[src_index + 2];
                }
            }
        }
        x += 9;
    }
}

void
raster_hline(
    uint8_t *framebuffer, uint32_t width, uint32_t height,
    int32_t y, int32_t x0, int32_t x1,
    uint8_t r, uint8_t g, uint8_t b)
{
    r *= 0.2f;
    g *= 0.2f;
    b *= 0.2f;

    for (int32_t x = x0; x < x1; x++)
    {
        int32_t index = (x + y * width) * 4;
        framebuffer[index    ] += b;
        framebuffer[index + 1] += g;
        framebuffer[index + 2] += r;
    }
}

void
raster_vline(
    uint8_t *framebuffer, uint32_t width, uint32_t height,
    int32_t x, int32_t y0, int32_t y1,
    uint8_t r, uint8_t g, uint8_t b)
{
    r *= 0.2f;
    g *= 0.2f;
    b *= 0.2f;

    for (int32_t y = y0; y < y1; y++)
    {
        int32_t index = (x + y * width) * 4;
        framebuffer[index    ] += b;
        framebuffer[index + 1] += g;
        framebuffer[index + 2] += r;
    }
}

void
raster_grid(uint8_t *framebuffer, uint32_t width, uint32_t height)
{
    for (uint32_t i = 10; i < height; i += 10)
    {
        raster_hline(
            framebuffer, width, height,
            i, 0, width,
            30, 50, 10);
    }
    for (uint32_t i = 100; i < height; i += 100)
    {
        raster_hline(
            framebuffer, width, height,
            i, 0, width,
            60, 100, 20);
    }
    for (uint32_t i = 10; i < width; i += 10)
    {
        raster_vline(
            framebuffer, width, height,
            i, 0, height,
            30, 50, 10);
    }
    for (uint32_t i = 100; i < width; i += 100)
    {
        raster_vline(
            framebuffer, width, height,
            i, 0, height,
            60, 100, 20);
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
    else if (y2 <= y0 && y2 <= y1)
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
    else
    {
        assert(false);
    }

    float m1 = (float)(max_x - min_x) / (float)(max_y - min_y);
    float m2 = (float)(mid_x - min_x) / (float)(mid_y - min_y);
    float m3 = (float)(max_x - mid_x) / (float)(max_y - mid_y);

    float xl = min_x;
    float xr = min_x;

    if (m2 > m1)
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
