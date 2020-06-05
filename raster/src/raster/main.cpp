#include "raster/Canvas.h"

int WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nShowCmd
)
{
    Canvas canvas = canvas_new("raster", 1280, 720);
    for (uint32_t i = 0; i < canvas.width * canvas.height * 4; i += 4)
    {
        canvas.framebuffer[i    ] = 150;
        canvas.framebuffer[i + 1] = 100;
        canvas.framebuffer[i + 2] = 50;
    }

    bool mouse_down = false;
    int32_t mouse_x, mouse_y;
    while (canvas_loop(canvas, mouse_down, mouse_x, mouse_y))
    {
        if (mouse_down && mouse_x > 0 && mouse_x < canvas.width && mouse_y > 0 && mouse_y < canvas.height)
        {
            canvas.framebuffer[(mouse_x + canvas.width * mouse_y) * 4    ] = 55;
            canvas.framebuffer[(mouse_x + canvas.width * mouse_y) * 4 + 1] = 155;
            canvas.framebuffer[(mouse_x + canvas.width * mouse_y) * 4 + 2] = 255;
        }
        canvas_flush(canvas);
    }

    canvas_free(canvas);
}
