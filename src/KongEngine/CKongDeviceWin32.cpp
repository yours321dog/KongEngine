// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

int screen_exit = 0;

#include "CKongDeviceWin32.h"
// win32 event handler
LRESULT CALLBACK screen_events(HWND hWnd, UINT msg,
    WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_CLOSE: screen_exit = 1; break;
    default: return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}


namespace kong
{
    namespace video
    {
#ifdef _KONG_COMPILE_WITH_DIRECT3D_8_
        IVideoDriver* createDirectX8Driver(const kong::SIrrlichtCreationParameters& params,
            io::IFileSystem* io, HWND window);
#endif

#ifdef _KONG_COMPILE_WITH_DIRECT3D_9_
        IVideoDriver* createDirectX9Driver(const kong::SIrrlichtCreationParameters& params,
            io::IFileSystem* io, HWND window);
#endif

#ifdef _KONG_COMPILE_WITH_OPENGL_
        IVideoDriver* CreateOpenGLDriver(const kong::SKongCreationParameters& params,
            /*io::IFileSystem* io,*/ CKongDeviceWin32* device);
#endif
    }
} // end namespace kong


namespace kong
{

    //CKongDeviceWin32::CKongDeviceWin32(const SKongCreationParameters &param)
    //    : CKongDeviceStub(param)
    //{
    //    screen_w = 0;
    //    screen_h = 0;
    //    screen_exit = 0;
    //    screen_mx = 0;
    //    screen_my = 0;
    //    screen_mb = 0;
    //    screen_handle = NULL;
    //    screen_dc = NULL;
    //    screen_hb = NULL;
    //    screen_ob = NULL;
    //    screen_fb = NULL;
    //    screen_pitch = 0;

    //    s32 w = param.window_size_.width_;
    //    s32 h = param.window_size_.height_;

    //    WNDCLASS wc = { CS_BYTEALIGNCLIENT, (WNDPROC)screen_events, 0, 0, 0,
    //        NULL, NULL, NULL, NULL, __TEXT("SCREEN3.1415926") };
    //    BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB,
    //        w * h * 4, 0, 0, 0, 0 } };
    //    RECT rect = { 0, 0, w, h };
    //    int wx, wy, sx, sy;
    //    LPVOID ptr;
    //    HDC hDC;

    //    ScreenClose();

    //    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    //    wc.hInstance = GetModuleHandle(NULL);
    //    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    //    if (!RegisterClass(&wc)) return;

    //    screen_handle = CreateWindow(__TEXT("SCREEN3.1415926"), __TEXT("AAAA"),
    //        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
    //        0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
    //    if (screen_handle == NULL) return;

    //    screen_exit = 0;
    //    hDC = GetDC(screen_handle);
    //    screen_dc = CreateCompatibleDC(hDC);
    //    ReleaseDC(screen_handle, hDC);

    //    screen_hb = CreateDIBSection(screen_dc, &bi, DIB_RGB_COLORS, &ptr, 0, 0);
    //    if (screen_hb == NULL) return;

    //    screen_ob = (HBITMAP)SelectObject(screen_dc, screen_hb);
    //    screen_fb = (unsigned char*)ptr;
    //    screen_w = w;
    //    screen_h = h;
    //    screen_pitch = w * 4;

    //    AdjustWindowRect(&rect, GetWindowLong(screen_handle, GWL_STYLE), 0);
    //    wx = rect.right - rect.left;
    //    wy = rect.bottom - rect.top;
    //    sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
    //    sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;
    //    if (sy < 0) sy = 0;
    //    SetWindowPos(screen_handle, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
    //    SetForegroundWindow(screen_handle);

    //    ShowWindow(screen_handle, SW_NORMAL);
    //    ScreenDispatch();

    //    memset(screen_fb, 0, w * h * 4);

    //    return;
    //}

    CKongDeviceWin32::CKongDeviceWin32(const SKongCreationParameters &param)
        : CKongDeviceStub(param)
    {
        // Create a window to test antialiasing support
        const kong::fschar_t* ClassName = __TEXT("GLCKongDeviceWin32");
        HINSTANCE lhInstance = GetModuleHandle(0);

        // Register Class
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        //wcex.lpfnWndProc = static_cast<WNDPROC>(DefWindowProc);
        wcex.lpfnWndProc = static_cast<WNDPROC>(screen_events);
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = lhInstance;
        wcex.hIcon = nullptr;
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = ClassName;
        wcex.hIconSm = nullptr;
        wcex.hIcon = nullptr;
        RegisterClassEx(&wcex);

        RECT client_size;
        client_size.top = 0;
        client_size.left = 0;
        client_size.right = param.window_size_.width_;
        client_size.bottom = param.window_size_.height_;

        DWORD style = WS_POPUP;
        if (!param.fullscreen_)
            style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

        AdjustWindowRect(&client_size, style, FALSE);

        const s32 realWidth = client_size.right - client_size.left;
        const s32 realHeight = client_size.bottom - client_size.top;

        s32 windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
        s32 windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

        if (windowLeft < 0)
            windowLeft = 0;
        if (windowTop < 0)
            windowTop = 0;	// make sure window menus are in screen on creation

        if (create_params_.fullscreen_)
        {
            windowLeft = 0;
            windowTop = 0;
        }

        HWND temporary_wnd = CreateWindow(ClassName, __TEXT(""), style, windowLeft,
            windowTop, realWidth, realHeight, NULL, NULL, lhInstance, NULL);

        create_params_.window_id_ = temporary_wnd;

        ShowWindow(temporary_wnd, SW_SHOWNORMAL);
        UpdateWindow(temporary_wnd);

        // fix ugly ATI driver bugs. Thanks to ariaci
        MoveWindow(temporary_wnd, windowLeft, windowTop, realWidth, realHeight, TRUE);

        screen_handle = temporary_wnd;
        return;
    }

    CKongDeviceWin32::~CKongDeviceWin32()
    {
    }

    bool CKongDeviceWin32::run()
    {
        ScreenDispatch();

        if (screen_exit == 1)
        {
            close_ = true;
        }

        return !close_;
    }

    void CKongDeviceWin32::ScreenClose()
    {
        if (screen_dc) {
            if (screen_ob) {
                SelectObject(screen_dc, screen_ob);
                screen_ob = NULL;
            }
            DeleteDC(screen_dc);
            screen_dc = NULL;
        }
        if (screen_hb) {
            DeleteObject(screen_hb);
            screen_hb = NULL;
        }
        if (screen_handle) {
            CloseWindow(screen_handle);
            screen_handle = NULL;
        }
    }

    void CKongDeviceWin32::ScreenDispatch()
    {
        MSG msg;
        while (true) {
            if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) break;
            if (!GetMessage(&msg, NULL, 0, 0)) break;
            DispatchMessage(&msg);
        }
    }

    void CKongDeviceWin32::CreateVideo()
    {
        video_driver_ = video::CreateOpenGLDriver(create_params_/*, FileSystem*/, this);
    }
}
