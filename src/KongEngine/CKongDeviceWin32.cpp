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

    CKongDeviceWin32::CKongDeviceWin32(const SKongCreationParameters &param)
        : CKongDeviceStub(param)
    {
        screen_w = 0;
        screen_h = 0;
        screen_exit = 0;
        screen_mx = 0;
        screen_my = 0;
        screen_mb = 0;
        screen_handle = NULL;
        screen_dc = NULL;
        screen_hb = NULL;
        screen_ob = NULL;
        screen_fb = NULL;
        screen_pitch = 0;

        s32 w = param.window_size_.width_;
        s32 h = param.window_size_.height_;

        WNDCLASS wc = { CS_BYTEALIGNCLIENT, (WNDPROC)screen_events, 0, 0, 0,
            NULL, NULL, NULL, NULL, __TEXT("SCREEN3.1415926") };
        BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB,
            w * h * 4, 0, 0, 0, 0 } };
        RECT rect = { 0, 0, w, h };
        int wx, wy, sx, sy;
        LPVOID ptr;
        HDC hDC;

        ScreenClose();

        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.hInstance = GetModuleHandle(NULL);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        if (!RegisterClass(&wc)) return;

        screen_handle = CreateWindow(__TEXT("SCREEN3.1415926"), __TEXT("AAAA"),
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
        if (screen_handle == NULL) return;

        screen_exit = 0;
        hDC = GetDC(screen_handle);
        screen_dc = CreateCompatibleDC(hDC);
        ReleaseDC(screen_handle, hDC);

        screen_hb = CreateDIBSection(screen_dc, &bi, DIB_RGB_COLORS, &ptr, 0, 0);
        if (screen_hb == NULL) return;

        screen_ob = (HBITMAP)SelectObject(screen_dc, screen_hb);
        screen_fb = (unsigned char*)ptr;
        screen_w = w;
        screen_h = h;
        screen_pitch = w * 4;

        AdjustWindowRect(&rect, GetWindowLong(screen_handle, GWL_STYLE), 0);
        wx = rect.right - rect.left;
        wy = rect.bottom - rect.top;
        sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
        sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;
        if (sy < 0) sy = 0;
        SetWindowPos(screen_handle, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
        SetForegroundWindow(screen_handle);

        ShowWindow(screen_handle, SW_NORMAL);
        ScreenDispatch();

        memset(screen_fb, 0, w * h * 4);

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
}
