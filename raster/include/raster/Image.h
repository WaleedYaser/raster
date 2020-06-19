#pragma once

struct Image
{
    int w, h, c;
    float *data;
};

Image
image_new(int w, int h, int c);

Image
image_load(const char *filename, int channels = 0);

void
image_free(Image &self);

void
image_save(const Image &self, const char *filename);

Image
image_clone(const Image &self);

Image
image_rgb_to_grayscale(const Image &self);

Image
image_rgb_to_hsv(const Image &self);

Image
image_hsv_to_rgb(const Image &self);

void
image_shift(Image &self, int c, float v);

void
image_clamp(Image &self);

inline bool
between(int n, int min, int max)
{
    return (n >= min && n <= max);
}

inline int
clamp(int n, int min, int max)
{
    if (n < min)
        return min;
    else if (n > max)
        return max;

    return n;
}

inline float
image_pixel(const Image &self, int x, int y, int c)
{
    x = clamp(x, 0, self.w - 1);
    y = clamp(y, 0, self.h - 1);

    int index = x + y * self.w + c * self.w * self.h;
    return self.data[index];
}

inline void
image_pixel_set(Image &self, int x, int y, int c, float v)
{
    if (between(x, 0, self.w - 1) && between(y, 0, self.h - 1))
    {
        int index = x + y * self.w + c * self.w * self.h;
        self.data[index] = v;
    }
}
