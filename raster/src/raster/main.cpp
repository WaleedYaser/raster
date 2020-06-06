#include "raster/Canvas.h"
#include "raster/Raster.h"

int WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nShowCmd
)
{
    Canvas canvas = canvas_new("raster", 1280, 720);

    bool mouse_down = false;
    int32_t mouse_x, mouse_y;
    int32_t mouse_x_old = 0;
    int32_t mouse_y_old = 0;
    bool init = true;
    while (canvas_loop(canvas, mouse_down, mouse_x, mouse_y))
    {
        if (mouse_down && mouse_x > 0 && mouse_x < canvas.width && mouse_y > 0 && mouse_y < canvas.height)
        {
            if (init)
            {
                mouse_x_old = mouse_x;
                mouse_y_old = mouse_y;
                init = false;
            }

            canvas.framebuffer[(mouse_x + canvas.width * mouse_y) * 4    ] = 55;
            canvas.framebuffer[(mouse_x + canvas.width * mouse_y) * 4 + 1] = 155;
            canvas.framebuffer[(mouse_x + canvas.width * mouse_y) * 4 + 2] = 255;

            raster_line(
                canvas.framebuffer, canvas.width, canvas.height,
                mouse_x_old, mouse_y_old, mouse_x, mouse_y,
                255, 155, 55);

            mouse_x_old = mouse_x;
            mouse_y_old = mouse_y;
        }
        else
        {
            init = true;
        }
        canvas_flush(canvas);
    }

    canvas_free(canvas);
}
