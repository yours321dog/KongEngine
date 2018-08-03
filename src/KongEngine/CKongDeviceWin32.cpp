// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "IEventReceiver.h"
#include "List.h"
int screen_exit = 0;

#include "CKongDeviceWin32.h"
// win32 event handler


namespace
{
    struct SEnvMapper
    {
        HWND hWnd;
        kong::CKongDeviceWin32* irrDev;
    };
    kong::core::List<SEnvMapper> EnvMap;

    HKL KEYBOARD_INPUT_HKL = 0;
    unsigned int KEYBOARD_INPUT_CODEPAGE = 1252;
}

SEnvMapper* GetEnvMapperFromHWnd(HWND hWnd)
{
    kong::core::List<SEnvMapper>::Iterator it = EnvMap.begin();
    for (; it != EnvMap.end(); ++it)
        if ((*it).hWnd == hWnd)
            return &(*it);

    return nullptr;
}


kong::CKongDeviceWin32* GetDeviceFromHWnd(HWND hWnd)
{
    kong::core::List<SEnvMapper>::Iterator it = EnvMap.begin();
    for (; it != EnvMap.end(); ++it)
        if ((*it).hWnd == hWnd)
            return (*it).irrDev;

    return nullptr;
}


LRESULT CALLBACK ScreenEvents(HWND hWnd, UINT msg,
    WPARAM wParam, LPARAM lParam)
{
    kong::SEvent event;
    kong::CKongDeviceWin32* dev = nullptr;

    switch (msg) {
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP:
    {
        BYTE allKeys[256];

        event.EventType = kong::EET_KEY_INPUT_EVENT;
        event.KeyInput.Key = static_cast<kong::EKEY_CODE>(wParam);
        event.KeyInput.PressedDown = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);

        const UINT MY_MAPVK_VSC_TO_VK_EX = 3; // MAPVK_VSC_TO_VK_EX should be in SDK according to MSDN, but isn't in mine.
        if (event.KeyInput.Key == kong::KEY_SHIFT)
        {
            // this will fail on systems before windows NT/2000/XP, not sure _what_ will return there instead.
            event.KeyInput.Key = (kong::EKEY_CODE)MapVirtualKey(((lParam >> 16) & 255), MY_MAPVK_VSC_TO_VK_EX);
        }
        if (event.KeyInput.Key == kong::KEY_CONTROL)
        {
            event.KeyInput.Key = (kong::EKEY_CODE)MapVirtualKey(((lParam >> 16) & 255), MY_MAPVK_VSC_TO_VK_EX);
            // some keyboards will just return LEFT for both - left and right keys. So also check extend bit.
            if (lParam & 0x1000000)
                event.KeyInput.Key = kong::KEY_RCONTROL;
        }
        if (event.KeyInput.Key == kong::KEY_MENU)
        {
            event.KeyInput.Key = (kong::EKEY_CODE)MapVirtualKey(((lParam >> 16) & 255), MY_MAPVK_VSC_TO_VK_EX);
            if (lParam & 0x1000000)
                event.KeyInput.Key = kong::KEY_RMENU;
        }

        GetKeyboardState(allKeys);

        event.KeyInput.Shift = ((allKeys[VK_SHIFT] & 0x80) != 0);
        event.KeyInput.Control = ((allKeys[VK_CONTROL] & 0x80) != 0);

        //// Handle unicode and deadkeys in a way that works since Windows 95 and nt4.0
        //// Using ToUnicode instead would be shorter, but would to my knowledge not run on 95 and 98.
        //WORD keyChars[2];
        //UINT scanCode = HIWORD(lParam);
        //int conversionResult = ToAsciiEx(wParam, scanCode, allKeys, keyChars, 0, KEYBOARD_INPUT_HKL);
        //if (conversionResult == 1)
        //{
        //    WORD unicodeChar;
        //    MultiByteToWideChar(
        //        KEYBOARD_INPUT_CODEPAGE,
        //        MB_PRECOMPOSED, // default
        //        (LPCSTR)keyChars,
        //        sizeof(keyChars),
        //        (WCHAR*)&unicodeChar,
        //        1);
        //    event.KeyInput.Char = unicodeChar;
        //}
        //else
        //    event.KeyInput.Char = 0;

        // allow composing characters like '@' with Alt Gr on non-US keyboards
        if ((allKeys[VK_MENU] & 0x80) != 0)
            event.KeyInput.Control = 0;

        dev = GetDeviceFromHWnd(hWnd);
        if (dev)
            dev->PostEventFromUser(event);

        if (msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP)
            return DefWindowProc(hWnd, msg, wParam, lParam);
        else
            return 0;
    }
    case WM_CLOSE: 
        screen_exit = 1; 
        break;
    default: 
        return DefWindowProc(hWnd, msg, wParam, lParam);
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
            io::IFileSystem* io, CKongDeviceWin32* device);

        IVideoDriver* CreateOpenGLShaderDriver(const kong::SKongCreationParameters& params,
            io::IFileSystem* io, CKongDeviceWin32* device);

        IVideoDriver* CreateOpenGLDeferredShaderDriver(const kong::SKongCreationParameters& params,
            io::IFileSystem* io, CKongDeviceWin32* device);
#endif
    }
} // end namespace kong


namespace kong
{
    CKongDeviceWin32::CKongDeviceWin32(const SKongCreationParameters &param)
        : CKongDeviceStub(param)
    {
        // Create a window to test antialiasing support
        const kong::fschar_t* ClassName = __TEXT("CKongDeviceWin32");
        HINSTANCE lhInstance = GetModuleHandle(0);

        // Register Class
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        //wcex.lpfnWndProc = static_cast<WNDPROC>(DefWindowProc);
        wcex.lpfnWndProc = static_cast<WNDPROC>(ScreenEvents);
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

        CreateVideo();

        if (video_driver_ != nullptr)
        {
            CreateScene();
        }

        SEnvMapper em;
        em.irrDev = this;
        em.hWnd = temporary_wnd;
        EnvMap.push_back(em);

        return;
    }

    CKongDeviceWin32::~CKongDeviceWin32()
    {
    }

    bool CKongDeviceWin32::run()
    {
        StartEventFromUser();

        ScreenDispatch();

        if (screen_exit == 1)
        {
            close_ = true;
        }

        return !close_;
    }

    video::IVideoDriver* CKongDeviceWin32::GetVideoDriver()
    {
        return video_driver_;
    }

    scene::ISceneManager* CKongDeviceWin32::GetSceneManager()
    {
        return scene_manager_;
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
        //video_driver_ = video::CreateOpenGLDriver(create_params_, file_system_, this);
        video_driver_ = video::CreateOpenGLShaderDriver(create_params_, file_system_, this);
        //video_driver_ = video::CreateOpenGLDeferredShaderDriver(create_params_, file_system_, this);
    }
}
