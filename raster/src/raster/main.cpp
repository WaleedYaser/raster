#include "raster/Canvas.h"
#include "raster/Raster.h"

#include <Windows.h>
#include <vector>

int WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nShowCmd
)
{
    Canvas canvas = canvas_new("raster", 1280, 720);

    std::vector<std::vector<int32_t>> mouse_segments;
    std::vector<int32_t> triangles;

    bool paint = false;
    bool grid  = false;

    float scale = 1.0f;
    while (canvas_loop(canvas))
    {
        char buffer[1000];
        wsprintf(buffer, "%d\n", canvas.input.mouse_wheel);
        OutputDebugString(buffer);

        if (canvas.input.key_g.pressed)
            grid = !grid;
        if (canvas.input.key_p.pressed)
            paint = !paint;

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

        // rendering
        raster_clear(canvas.framebuffer, canvas.width, canvas.height, 30, 30, 30);

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
