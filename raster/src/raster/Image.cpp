#include "raster/Image.h"
#include "raster/stb_image.h"
#include "raster/stb_image_write.h"

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

Image
image_new(int w, int h, int c)
{
    Image self;

    self.data = (float *) calloc(h * w * c, sizeof(float));
    assert(self.data && "calloc falied");
    self.w = w;
    self.h = h;
    self.c = c;

    return self;
}

Image
image_load(const char *filename, int channels)
{
    int w, h, c;
    uint8_t *data = stbi_load(filename, &w, &h, &c, channels);
    if (!data)
    {
        fprintf(stderr, "Cannot load image \"%s\"\nSTB Reason: %s\n", filename, stbi_failure_reason());
        exit(0);
    }

    if (channels)
        c = channels;

    Image self = image_new(w, h, c);
    for (int k = 0; k < c; ++k)
    {
        for (int j = 0; j < h; ++j)
        {
            for (int i = 0; i < w; ++i)
            {
                int dst_index = i + w * j + w * h * k;
                int src_index = k + c * i + c * w * j;
                self.data[dst_index] = (float)data[src_index] / 255.0f;
            }
        }
    }

    // get rid of alpha
    if (self.c == 4)
        self.c = 3;

    free(data);
    return self;
}

void
image_free(Image &self)
{
    free(self.data);
    self = Image{};
}

void
image_save(const Image &self, const char *filename)
{
    uint8_t *data = (uint8_t *)calloc(self.w * self.h * self.c, sizeof(uint8_t));
    assert(self.data && "calloc falied");

    for (int k = 0; k < self.c; ++k)
    {
        for (int i = 0; i < self.w * self.h; ++i)
        {
            data[i * self.c + k] = (uint8_t) roundf((255 * self.data[i + k * self.w * self.h]));
        }
    }

    int success = stbi_write_jpg(filename, self.w, self.h, self.c, data, 100);
    free(data);

    if (!success)
        fprintf(stderr, "Failed to write image %s\n", filename);
}

Image
image_clone(const Image &self)
{
    Image other = image_new(self.w, self.h, self.c);
    ::memcpy(other.data, self.data, self.w * self.h * self.c * sizeof(float));
    return other;
}

Image
image_rgb_to_grayscale(const Image &self)
{
    assert(self.c == 3);
    Image gray = image_new(self.w, self.h, 1);

    for (int j = 0; j < self.h; ++j)
    {
        for (int i = 0; i < self.w; ++i)
        {
            float r = self.data[i + j * self.w + 0 * self.w * self.h];
            float g = self.data[i + j * self.w + 1 * self.w * self.h];
            float b = self.data[i + j * self.w + 2 * self.w * self.h];

            gray.data[i + j * self.w] = 0.299f * r + 0.587f * g + 0.114f * b;
        }
    }
    return gray;
}

Image &
image_rgb_to_hsv(Image &self)
{
    for (int i = 0; i < self.w * self.h; ++i)
    {
        float r = self.data[i + self.w * self.h * 0];
        float g = self.data[i + self.w * self.h * 1];
        float b = self.data[i + self.w * self.h * 2];

        float v = (r > g) ? ((r > b) ? r : b) : ((g > b) ? g : b);
        float m = (r < g) ? ((r < b) ? r : b) : ((g < b) ? g : b);
        float c = v - m;

        float s = 0;
        if (v != 0)
        {
            s = c / v;
        }

        float h = 0;
        if (c != 0)
        {
            if (v == r)
                h = (g - b) / c;
            else if (v == g)
                h = (b - r) / c + 2;
            else if (v == b)
                h = (r - g) / c + 4;

            if (h < 0)
                h = h / 6 + 1;
            else
                h = h / 6;
        }

        self.data[i + self.w * self.h * 0] = h;
        self.data[i + self.w * self.h * 1] = s;
        self.data[i + self.w * self.h * 2] = v;
    }
    return self;
}

Image &
image_hsv_to_rgb(Image &self)
{
    for (int i = 0; i < self.w * self.h; ++i)
    {
        float h = self.data[i + self.w * self.h * 0] * 6;
        float s = self.data[i + self.w * self.h * 1];
        float v = self.data[i + self.w * self.h * 2];

        float c = v * s;
        float x = c * (1 - fabs((fmod(h, 2) - 1.0f)));
        float m = v - c;

        float r, g, b;
        if (h >= 0 && h <= 1)
        {
            r = c + m;
            g = x + m;
            b = 0 + m;
        }
        else if (h > 1 && h <= 2)
        {
            r = x + m;
            g = c + m;
            b = 0 + m;
        }
        else if (h > 2 && h <= 3)
        {
            r = 0 + m;
            g = c + m;
            b = x + m;
        }
        else if (h > 3 && h <= 4)
        {
            r = 0 + m;
            g = x + m;
            b = c + m;
        }
        else if (h > 4 && h <= 5)
        {
            r = x + m;
            g = 0 + m;
            b = c + m;
        }
        else if (h > 5 && h <= 6)
        {
            r = c + m;
            g = 0 + m;
            b = x + m;
        }
        else
        {
            r = 0;
            g = 0;
            b = 0;
        }

        self.data[i + self.w * self.h * 0] = r;
        self.data[i + self.w * self.h * 1] = g;
        self.data[i + self.w * self.h * 2] = b;
    }

    return self;
}

Image &
image_shift(Image &self, int c, float v)
{
    for (int i = 0; i < self.w * self.h; ++i)
    {
        self.data[i + self.w * self.h * c] += v;
    }
    return self;
}

Image &
image_scale(Image &self, int c, float v)
{
    for (int i = 0; i < self.w * self.h; ++i)
    {
        self.data[i + self.w * self.h * c] *= v;
    }
    return self;
}

Image &
image_clamp(Image &self)
{
    for (int i = 0; i < self.w * self.h * self.c; ++i)
    {
        float v = self.data[i];
        if (v < 0) v = 0;
        else if (v > 1) v = 1.0f;
        self.data[i] = v;
    }
    return self;
}
