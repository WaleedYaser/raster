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

Image &
image_rgb_to_hsv(Image &self);

Image &
image_hsv_to_rgb(Image &self);

Image &
image_shift(Image &self, int c, float v);

Image &
image_scale(Image &self, int c, float v);

Image &
image_clamp(Image &self);

float
image_pixel(const Image &self, int x, int y, int c);

void
image_pixel_set(Image &self, int x, int y, int c, float v);

Image
image_channel(Image &self, int c);

bool
image_same(const Image &a, const Image &b);

Image
image_add(const Image &a, const Image &b);

Image
image_subtract(const Image &a, const Image &b);

float
image_interpolate_nn(const Image &self, float x, float y, int c);

float
image_interpolate_bilinear(const Image &self, float x, float y, int c);

Image
image_resize_nn(const Image &self, int w, int h);

Image
image_resize_bilinear(const Image &self, int w, int h);

Image
image_conv(const Image &self, const Image &filter, bool preserve);

Image
image_filter_box(int w);

Image
image_filter_highpass();

Image
image_filter_sharpen();

Image
image_filter_emboss();

Image
image_filter_guassian(float sigma);

Image
image_filter_gx();

Image
image_filter_gy();

void
image_feature_normalize(Image &self);

void
image_threshold(Image &self, float trheshold);

Image
image_sobel_magnitude(const Image &self);

Image
image_sobel_angle(const Image &self);

Image
image_sobel_colorize(const Image &self);
