#include "raster/Canvas.h"

#include <Windowsx.h>
#include <assert.h>

LRESULT CALLBACK
_wnd_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

Canvas
canvas_new(const char *title, uint32_t width, uint32_t height)
{
    WNDCLASSEXA wcx = {};
    wcx.cbSize        = sizeof(wcx);
    wcx.style         = CS_HREDRAW | CS_VREDRAW;
    wcx.lpfnWndProc   = _wnd_proc;
    wcx.lpszClassName = "canvas_class";

    ATOM class_atom = RegisterClassExA(&wcx);
    assert(class_atom != 0 && "RegisterClass failed");

    HWND hwnd = CreateWindowExA(
        0,
        MAKEINTATOM(class_atom),
        title,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        nullptr,
        nullptr,
        nullptr,
        nullptr);

    RECT rect;
    GetClientRect(hwnd, &rect);

    Canvas self;
    self.handle = hwnd;
    self.width  = rect.right - rect.left;
    self.height = rect.bottom - rect.top;

    self.framebuffer = (uint8_t *)VirtualAlloc(
        nullptr,
        self.width * self.height * 4,
        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    assert(self.framebuffer && "VirsualAlloc failed");

    return self;
}

void
canvas_free(Canvas &self)
{
    VirtualFree(self.framebuffer, 0, MEM_RELEASE);
    DestroyWindow(self.handle);
}

bool
canvas_loop(Canvas &self, bool &mouse_down, int32_t &mouse_x, int32_t &mouse_y)
{
    MSG msg = {};
    while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        switch (msg.message)
        {
        case WM_QUIT:
            return false;

        case WM_LBUTTONDOWN:
            mouse_down = true;
            break;
        case WM_LBUTTONUP:
            mouse_down = false;
            break;
        }
    }

    RECT rect;
    GetClientRect(self.handle, &rect);
    uint32_t width  = rect.right - rect.left;
    uint32_t height = rect.bottom - rect.top;

    if (width != self.width || height != self.height)
    {
        VirtualFree(self.framebuffer, 0, MEM_RELEASE);
        self.width  = width;
        self.height = height;
        self.framebuffer = (uint8_t *)VirtualAlloc(
            nullptr,
            self.width * self.height * 4,
            MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        assert(self.framebuffer && "VirsualAlloc failed");
    }

    POINT point;
    GetCursorPos(&point);
    ScreenToClient(self.handle, &point);
    mouse_x = point.x;
    mouse_y = point.y;

    return true;
}

void
canvas_flush(Canvas &self)
{
    HDC hdc = GetDC(self.handle);

    BITMAPINFOHEADER bmi_header = {};
    bmi_header.biSize        = sizeof(bmi_header);
    bmi_header.biWidth       = self.width;
    bmi_header.biHeight      = -self.height;
    bmi_header.biPlanes      = 1;
    bmi_header.biBitCount    = 32;
    bmi_header.biCompression = BI_RGB;

    BITMAPINFO bmi = {};
    bmi.bmiHeader = bmi_header;

    int res = StretchDIBits(
        hdc,
        0, 0, self.width, self.height,      // dest
        0, 0, self.width, self.height,      // src
        self.framebuffer,
        &bmi,
        DIB_RGB_COLORS,
        SRCCOPY);

    assert(res && "StretchDIBits failed");

    res = ReleaseDC(self.handle, hdc);
    assert(res && "ReleaseCC failed");
}