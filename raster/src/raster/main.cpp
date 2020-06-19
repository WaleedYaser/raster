#include "raster/Canvas.h"
#include "raster/Raster.h"
#include "raster/Cam.h"
#include "raster/Image.h"

#include "raster/Vec3.h"
#include "raster/Mat4.h"

#include <Windows.h>
#include <vector>

int WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nShowCmd
)
{
    Image im = image_load("data/dog.jpg");
    for (int j = 0; j < im.h; ++j)
        for (int i = 0; i < im.w; ++i)
            image_pixel_set(im, i, j, 0, 0);
    image_save(im, "dog_no_red.jpg");
    image_free(im);

    im = image_load("data/dog.jpg");
    image_shift(im, 0, 0.4f);
    image_shift(im, 1, 0.4f);
    image_shift(im, 2, 0.4f);
    image_save(im, "overflow.jpg");
    image_clamp(im);
    image_save(im, "fixed.jpg");
    image_free(im);

    im = image_load("data/colorbar.png");
    Image graybar = image_rgb_to_grayscale(im);
    image_save(graybar, "graybar.jpg");
    image_free(graybar);
    image_free(im);

    return true;

    Canvas canvas = canvas_new("raster", 1280, 720);

    std::vector<std::vector<int32_t>> mouse_segments;
    std::vector<int32_t> triangles;

    bool paint  = false;
    bool grid   = false;
    float scale = 1.0f;

    Cam cam = cam_new();

    while (canvas_loop(canvas))
    {
        char buffer[1000];
        wsprintf(buffer, "%d\n", canvas.input.mouse_wheel);
        OutputDebugString(buffer);

        if (canvas.input.key_g.pressed)
            grid = !grid;
        if (canvas.input.key_p.pressed)
            paint = !paint;

        if (canvas.input.key_space.down)
        {
            cam.t.position.z += canvas.input.mouse_dy;
        }


        if (paint)
        {
            if (canvas.input.key_space.pressed)
            {
                scale = 1.0f;
                mouse_segments.clear();
            }

            if (canvas.input.mouse_wheel > 0)
                scale += 0.2f;
            else if (canvas.input.mouse_wheel < 0 && scale > 0.3f)
                scale -= 0.2f;

            if (canvas.input.mouse_left.pressed)
            {
                mouse_segments.push_back(std::vector<int32_t>());
            }
            else if (canvas.input.mouse_left.down)
            {
                mouse_segments.back().push_back(canvas.input.mouse_x / scale);
                mouse_segments.back().push_back(canvas.input.mouse_y / scale);
            }
            else if (canvas.input.mouse_mid.down)
            {
                for (size_t i = 0; i < mouse_segments.size(); ++i)
                {
                    for (size_t j = 0; j + 1 < mouse_segments[i].size(); j += 2)
                    {
                        mouse_segments[i][j] += (canvas.input.mouse_dx / scale);
                        mouse_segments[i][j + 1] += (canvas.input.mouse_dy / scale);
                    }
                }
            }
        }

        else
        {
            if (canvas.input.mouse_left.pressed)
            {
                triangles.push_back(canvas.input.mouse_x);
                triangles.push_back(canvas.input.mouse_y);
            }
        }

        Mat4 view_proj = cam_view_proj(cam);
        // rendering
        raster_clear(canvas.framebuffer, canvas.width, canvas.height, 30, 30, 30);

        for (int i = -10; i <= 10; ++i)
        {
            Vec3 p0 = Vec3{(float)i, -10, -40};
            Vec3 p1 = Vec3{(float)i, -10, -20};

            p0 = mat4_transform_proj_point(view_proj, p0);
            p1 = mat4_transform_proj_point(view_proj, p1);

            if (
                p0.x >= -1 && p0.x <= 1 && p0.y >= -1 && p0.y <= 1 &&
                p1.x >= -1 && p1.x <= 1 && p1.y >= -1 && p1.y <= 1)
            {
                int32_t x0 = (int32_t)((p0.x + 1) * 0.5f * (canvas.width - 1));
                int32_t y0 = (int32_t)((1.0f - (p0.y + 1) * 0.5f) * (canvas.height - 1));
                int32_t x1 = (int32_t)((p1.x + 1) * 0.5f * (canvas.width - 1));
                int32_t y1 = (int32_t)((1.0f - (p1.y + 1) * 0.5f) * (canvas.height - 1));

                raster_line(
                    canvas.framebuffer, canvas.width, canvas.height,
                    x0, y0, x1, y1,
                    50, 100, 150);
            }
        }
        for (int i = -20; i >= -40; --i)
        {
            Vec3 p0 = Vec3{-10, -10, (float)(i)};
            Vec3 p1 = Vec3{ 10, -10, (float)(i)};

            p0 = mat4_transform_proj_point(view_proj, p0);
            p1 = mat4_transform_proj_point(view_proj, p1);

            if (
                p0.x >= -1 && p0.x <= 1 && p0.y >= -1 && p0.y <= 1 &&
                p1.x >= -1 && p1.x <= 1 && p1.y >= -1 && p1.y <= 1)
            {
                int32_t x0 = (int32_t)((p0.x + 1) * 0.5f * (canvas.width - 1));
                int32_t y0 = (int32_t)((1.0f - (p0.y + 1) * 0.5f) * (canvas.height - 1));
                int32_t x1 = (int32_t)((p1.x + 1) * 0.5f * (canvas.width - 1));
                int32_t y1 = (int32_t)((1.0f - (p1.y + 1) * 0.5f) * (canvas.height - 1));

                raster_line(
                    canvas.framebuffer, canvas.width, canvas.height,
                    x0, y0, x1, y1,
                    50, 100, 150);
            }
        }

        for (size_t i = 5; i < triangles.size(); i += 6)
        {
            raster_triangle(
                canvas.framebuffer, canvas.width, canvas.height,
                triangles[i - 5], triangles[i - 4],
                triangles[i - 3], triangles[i - 2],
                triangles[i - 1], triangles[i    ],
                100, 100, 100);

            raster_line(
                canvas.framebuffer, canvas.width, canvas.height,
                triangles[i - 5], triangles[i - 4],
                triangles[i - 3], triangles[i - 2],
                200, 150, 100);
            raster_line(
                canvas.framebuffer, canvas.width, canvas.height,
                triangles[i - 3], triangles[i - 2],
                triangles[i - 1], triangles[i    ],
                200, 150, 100);
            raster_line(
                canvas.framebuffer, canvas.width, canvas.height,
                triangles[i - 5], triangles[i - 4],
                triangles[i - 1], triangles[i    ],
                200, 150, 100);
        }

        if (paint)
        {
            for (const auto &v : mouse_segments)
            {
                if (v.size() > 3)
                {
                    for (size_t i = 2; i < v.size(); i += 2)
                    {
                        int32_t x0 = v[i - 2];
                        int32_t y0 = v[i - 1];

                        int32_t x1 = v[i];
                        int32_t y1 = v[i + 1];

                        if (
                            x0 * scale >= 0 && x0 * scale < canvas.width && y0 * scale >= 0 && y0 * scale < canvas.height &&
                            x1 * scale >= 0 && x1 * scale < canvas.width && y1 * scale >= 0 && y1 * scale < canvas.height)
                        {
                            raster_line(
                                canvas.framebuffer, canvas.width, canvas.height,
                                x0 * scale, y0 * scale, x1 * scale, y1 * scale,
                                200, 200, 200);
                        }
                    }
                }
            }
        }

        if (grid)
            raster_grid(canvas.framebuffer, canvas.width, canvas.height);

        canvas_flush(canvas);
    }

    canvas_free(canvas);
}
