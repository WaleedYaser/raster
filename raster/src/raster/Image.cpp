#include "raster/Image.h"
#include "raster/stb_image.h"
#include "raster/stb_image_write.h"

#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <math.h>
#include <vcruntime_string.h>

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

void
image_rgb_to_hsv(Image &self)
{
    // TODO
}

void
image_hsv_to_rgb(Image &self)
{
    // TODO
}

void
image_shift(Image &self, int c, float v)
{
    for (int i = 0; i < self.w * self.h; ++i)
    {
        self.data[i + self.w * self.h * c] += v;
    }
}

void
image_clamp(Image &self)
{
    for (int i = 0; i < self.w * self.h * self.c; ++i)
    {
        float v = self.data[i];
        if (v < 0) v = 0;
        else if (v > 1) v = 1.0f;
        self.data[i] = v;
    }
}
