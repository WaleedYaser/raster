#include "raster/Canvas.h"
#include "raster/Raster.h"

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

    bool new_segment = true;
    bool mouse_down  = false;
    int32_t mouse_x, mouse_y;
    while (canvas_loop(canvas, mouse_down, mouse_x, mouse_y))
    {
        raster_clear(canvas.framebuffer, canvas.width, canvas.height, 30, 30, 30);

        if (mouse_down)
        {
            if (new_segment)
            {
              mouse_segments.push_back(std::vector<int32_t>());
              new_segment = false;
            }

            mouse_segments.back().push_back(mouse_x);
            mouse_segments.back().push_back(mouse_y);
        }
        else
        {
            new_segment = true;
        }

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
                        x0 >= 0 && x0 < canvas.width && y0 >= 0 && y0 < canvas.height &&
                        x1 >= 0 && x1 < canvas.width && y1 >= 0 && y1 < canvas.height)
                    {
                        raster_line(
                            canvas.framebuffer, canvas.width, canvas.height,
                            x0, y0, x1, y1, 200, 200, 200);
                    }
                }
            }
        }

        canvas_flush(canvas);
    }

    canvas_free(canvas);
}
