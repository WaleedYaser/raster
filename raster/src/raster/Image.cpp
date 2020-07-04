#include "raster/Image.h"
#include "raster/stb_image.h"
#include "raster/stb_image_write.h"

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static constexpr float PI = 3.14159265359f;

inline bool
_between(int n, int min, int max)
{
    return (n >= min && n <= max);
}

inline int
_clamp(int n, int min, int max)
{
    if (n < min)
        return min;
    else if (n > max)
        return max;

    return n;
}

inline void
_image_l1_normalize(Image &self)
{
    float sum = 0;
    for (int i = 0; i < self.w * self.h * self.c; ++i)
    {
        sum += self.data[i];
    }

    sum = 1.0f / sum;
    for (int i = 0; i < self.w * self.h * self.c; ++i)
    {
        self.data[i] *= sum;
    }
}

// API

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

float
image_pixel(const Image &self, int x, int y, int c)
{
    x = _clamp(x, 0, self.w - 1);
    y = _clamp(y, 0, self.h - 1);

    int index = x + y * self.w + c * self.w * self.h;
    return self.data[index];
}

void
image_pixel_set(Image &self, int x, int y, int c, float v)
{
    if (_between(x, 0, self.w - 1) && _between(y, 0, self.h - 1))
    {
        int index = x + y * self.w + c * self.w * self.h;
        self.data[index] = v;
    }
}

Image
image_channel(Image &self, int c)
{
    // TODO(Waleed):
    return Image{};
}

bool
image_same(const Image &a, const Image &b)
{
    // TODO(Waleed):
    return false;
}

Image
image_add(const Image &a, const Image &b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    Image res = image_new(a.w, a.h, a.c);
    for (int i = 0; i < a.w * a.h * a.c; i++)
    {
        res.data[i] = a.data[i] + b.data[i];
    }
    return res;
}

Image
image_subtract(const Image &a, const Image &b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    Image res = image_new(a.w, a.h, a.c);
    for (int i = 0; i < a.w * a.h * a.c; i++)
    {
        res.data[i] = a.data[i] - b.data[i];
    }
    return res;
}

float
image_interpolate_nn(const Image &self, float x, float y, int c)
{
    return image_pixel(self, ::roundf(x), ::roundf(y), c);
}

float
image_interpolate_bilinear(const Image &self, float x, float y, int c)
{
    int xf = ::floorf(x);
    int xc = ::ceilf(x);
    int yf = ::floorf(y);
    int yc = ::ceilf(y);

    float v0 = image_pixel(self, xf, yf, c);
    float v1 = image_pixel(self, xc, yf, c);
    float v2 = image_pixel(self, xf, yc, c);
    float v3 = image_pixel(self, xc, yc, c);

    float dx = x - xf;
    float dy = y - yf;

    float i0 = dx * v1 + (1 - dx) * v0;
    float i1 = dx * v3 + (1 - dx) * v2;

    float res = i1 * dy + (1 - dy) * i0;

    return res;
}

Image
image_resize_nn(const Image &self, int w, int h)
{
    Image res = image_new(w, h, self.c);

    float ax = ((float)self.w) / ((float)res.w);
    float bx = 0.5f * ((float)(self.w - res.w)) / ((float)res.w);

    float ay = ((float)self.h) / ((float)res.h);
    float by = 0.5f * ((float)(self.h - res.h)) / ((float)res.h);

    for (int y = 0; y < res.h; ++y)
    {
        for (int x = 0; x < res.w; ++x)
        {
            float self_x = x * ax + bx;
            float self_y = y * ay + by;
            for (int c = 0; c < res.c; ++c)
            {
                image_pixel_set(res, x, y, c, image_interpolate_nn(self, self_x, self_y, c));
            }
        }
    }
    return res;
}

Image
image_resize_bilinear(const Image &self, int w, int h)
{
    Image res = image_new(w, h, self.c);

    float ax = ((float)self.w) / ((float)res.w);
    float bx = 0.5f * ((float)(self.w - res.w)) / ((float)res.w);

    float ay = ((float)self.h) / ((float)res.h);
    float by = 0.5f * ((float)(self.h - res.h)) / ((float)res.h);

    for (int y = 0; y < res.h; ++y)
    {
        for (int x = 0; x < res.w; ++x)
        {
            float self_x = x * ax + bx;
            float self_y = y * ay + by;
            for (int c = 0; c < res.c; ++c)
            {
                image_pixel_set(res, x, y, c, image_interpolate_bilinear(self, self_x, self_y, c));
            }
        }
    }
    return res;
}

Image
image_conv(const Image &self, const Image &filter, bool preserve)
{
    Image res = image_new(self.w, self.h, preserve ? self.c : 1);

    if (filter.c == self.c || filter.c == 1)
    {
        if (preserve)
        {
            for (int c = 0; c < self.c; ++c)
            {
                for (int y = 0; y < self.h; ++y)
                {
                    for (int x = 0; x < self.w; ++x)
                    {
                        float v = 0;
                        for (int yy = 0; yy < filter.h; ++yy)
                        {
                            for (int xx = 0; xx < filter.w; ++xx)
                            {
                                v += image_pixel(filter, xx, yy, filter.c == 1 ? 0 : c) *
                                     image_pixel(self, x + xx - filter.w / 2, y + yy - filter.h / 2, c);
                            }
                        }
                        image_pixel_set(res, x, y, c, v);
                    }
                }
            }
        }
        else
        {
            for (int y = 0; y < self.h; ++y)
            {
                for (int x = 0; x < self.w; ++x)
                {
                    float v = 0;
                    for (int yy = 0; yy < filter.h; ++yy)
                    {
                        for (int xx = 0; xx < filter.w; ++xx)
                        {
                            for (int c = 0; c < self.c; ++c)
                            {
                                v += image_pixel(filter, xx, yy, filter.c == 1 ? 0 : c) *
                                     image_pixel(self, x + xx - filter.w / 2, y + yy - filter.h / 2, c);
                            }
                        }
                    }
                    image_pixel_set(res, x, y, 0, v);
                }
            }
        }
    }
    else
    {
        assert(false && "invalid filter size");
    }

    return res;
}

Image
image_filter_box(int w)
{
    Image res = image_new(w, w, 1);
    float v = 1.0f / (float)(w * w);
    for (int i = 0; i < w * w; ++i)
        res.data[i] = v;

    return res;
}

Image
image_filter_highpass()
{
    Image res = image_new(3, 3, 1);
    float k[] = {
         0, -1,  0,
        -1,  4, -1,
         0, -1,  0};
    for (int i = 0; i < 9; ++i)
        res.data[i] = k[i];
    return res;
}

Image
image_filter_sharpen()
{
    Image res = image_new(3, 3, 1);
    float k[] = {
         0, -1,  0,
        -1,  5, -1,
         0, -1,  0};
    for (int i = 0; i < 9; ++i)
        res.data[i] = k[i];
    return res;
}

Image
image_filter_emboss()
{
    Image res = image_new(3, 3, 1);
    float k[] = {
        -2, -1,  0,
        -1,  1,  1,
         0,  1,  2};
    for (int i = 0; i < 9; ++i)
        res.data[i] = k[i];
    return res;
}

Image
image_filter_guassian(float sigma)
{
    Image res = image_new(6 * sigma + 1, 6 * sigma + 1, 1);

    float c1 = 1.0f / (float)(2 * sigma * sigma);
    float c2 = c1 / PI;
    for (int y = 0; y < res.h; ++y)
    {
        for (int x = 0; x < res.w; ++x)
        {
            int xx = -res.w / 2 + x;
            int yy = -res.h / 2 + y;
            float v = c2 * ::expf(-(xx * xx + yy * yy) * c1);
            res.data[x + y * res.w] = v;
        }
    }

    _image_l1_normalize(res);

    return res;
}

Image
image_filter_gx()
{
    return Image{};
}

Image
image_filter_gy()
{
    return Image{};
}

void
image_feature_normalize(Image self)
{
}

void
image_threshold(Image self, float trheshold)
{
}

Image
image_sobel(Image self)
{
    return Image{};
}

Image
image_sobel_colorize(Image self)
{
    return Image{};
}
