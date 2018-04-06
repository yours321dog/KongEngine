// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "COpenGLDriver.h"

#include "GL/gl.h"
//#include "GL/glew.h"

kong::video::COpenGLDriver::COpenGLDriver(const SKongCreationParameters& params, CKongDeviceWin32* device)
    : hdc_(nullptr), window_(static_cast<HWND>(params.window_id_)), hrc_(nullptr), device_(device), params_(params)
{
}

bool kong::video::COpenGLDriver::InitDriver(CKongDeviceWin32* device)
{
    //// Create a window to test antialiasing support
    //const kong::fschar_t* ClassName = __TEXT("GLCKongDeviceWin32");
    //HINSTANCE lhInstance = GetModuleHandle(0);

    //// Register Class
    //WNDCLASSEX wcex;
    //wcex.cbSize = sizeof(WNDCLASSEX);
    //wcex.style = CS_HREDRAW | CS_VREDRAW;
    //wcex.lpfnWndProc = static_cast<WNDPROC>(DefWindowProc);
    //wcex.cbClsExtra = 0;
    //wcex.cbWndExtra = 0;
    //wcex.hInstance = lhInstance;
    //wcex.hIcon = nullptr;
    //wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    //wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    //wcex.lpszMenuName = nullptr;
    //wcex.lpszClassName = ClassName;
    //wcex.hIconSm = nullptr;
    //wcex.hIcon = nullptr;
    //RegisterClassEx(&wcex);

    //RECT client_size;
    //client_size.top = 0;
    //client_size.left = 0;
    //client_size.right = params_.window_size_.width_;
    //client_size.bottom = params_.window_size_.height_;

    //DWORD style = WS_POPUP;
    //if (!params_.fullscreen_)
    //    style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

    //AdjustWindowRect(&client_size, style, FALSE);

    //const s32 realWidth = client_size.right - client_size.left;
    //const s32 realHeight = client_size.bottom - client_size.top;

    //const s32 windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
    //const s32 windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

    //HWND temporary_wnd = CreateWindow(ClassName, __TEXT(""), style, windowLeft,
    //    windowTop, realWidth, realHeight, NULL, NULL, lhInstance, NULL);

    //if (!temporary_wnd)
    //{
    //    //os::Printer::log("Cannot create a temporary window.", ELL_ERROR);
    //    UnregisterClass(ClassName, lhInstance);
    //    return false;
    //}

    //hdc_ = GetDC(temporary_wnd);
    
    // Set up pixel format descriptor with desired parameters
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),                  // Size Of This Pixel Format Descriptor
        1,                                              // Version Number
        PFD_DRAW_TO_WINDOW |                            // Format Must Support Window
        PFD_SUPPORT_OPENGL |                            // Format Must Support OpenGL
        PFD_DOUBLEBUFFER,                               // Format Must Support Double Buffering
        //(Params.Doublebuffer ? PFD_DOUBLEBUFFER : 0) |  // Must Support Double Buffering
        //(Params.Stereobuffer ? PFD_STEREO : 0),         // Must Support Stereo Buffer
        PFD_TYPE_RGBA,                                  // Request An RGBA Format
        params_.color_bits_,                            // Select Our Color Depth
        0, 0, 0, 0, 0, 0,                               // Color Bits Ignored
        0,                                              // No Alpha Buffer
        0,                                              // Shift Bit Ignored
        0,                                              // No Accumulation Buffer
        0, 0, 0, 0,	                                    // Accumulation Bits Ignored
        params_.z_buffer_bits_,                         // Z-Buffer (Depth Buffer)
        BYTE(params_.stencil_buffer_ ? 1 : 0),          // Stencil Buffer Depth
        0,                                              // No Auxiliary Buffer
        PFD_MAIN_PLANE,                                 // Main Drawing Layer
        0,                                              // Reserved
        0, 0, 0                                         // Layer Masks Ignored
    };

    GLuint pixel_format = 0;

    //for (u32 i = 0; i<6; ++i)
    //{
    //    if (i == 1)
    //    {
    //        if (params_.stencil_buffer_)
    //        {
    //            //os::Printer::log("Cannot create a GL device with stencil buffer, disabling stencil shadows.", ELL_WARNING);
    //            params_.stencil_buffer_ = false;
    //            pfd.cStencilBits = 0;
    //        }
    //        else
    //            continue;
    //    }
    //    else if (i == 2)
    //    {
    //        pfd.cDepthBits = 24;
    //    }
    //    else
    //    if (i == 3)
    //    {
    //        if (params_.color_bits_ != 16)
    //            pfd.cColorBits = 16;
    //        else
    //            continue;
    //    }
    //    else if (i == 4)
    //    {
    //        // try single buffer
    //        //if (params_.Doublebuffer)
    //            pfd.dwFlags &= ~PFD_DOUBLEBUFFER;
    //        //else
    //            //continue;
    //    }
    //    else if (i == 5)
    //    {
    //        //os::Printer::log("Cannot create a GL device context", "No suitable format for temporary window.", ELL_ERROR);
    //        ReleaseDC(temporary_wnd, hdc_);
    //        DestroyWindow(temporary_wnd);
    //        UnregisterClass(ClassName, lhInstance);
    //        return false;
    //    }

    //    // choose pixelformat
    //    pixel_format = ChoosePixelFormat(hdc_, &pfd);
    //    if (pixel_format)
    //    {
    //        break;
    //    }
    //}

    //SetPixelFormat(hdc_, pixel_format, &pfd);

    //HGLRC hrc = wglCreateContext(hdc_);
    //if (!hrc)
    //{
    //    //os::Printer::log("Cannot create a temporary GL rendering context.", ELL_ERROR);
    //    ReleaseDC(temporary_wnd, hdc_);
    //    DestroyWindow(temporary_wnd);
    //    UnregisterClass(ClassName, lhInstance);
    //    return false;
    //}

    //wglMakeCurrent(hdc_, NULL);
    //wglDeleteContext(hrc);
    //ReleaseDC(temporary_wnd, hdc_);
    //DestroyWindow(temporary_wnd);
    //UnregisterClass(ClassName, lhInstance);

    // windows created by device
    // get hdc
    hdc_ = GetDC(window_);
    if (!hdc_)
    {
        //os::Printer::log("Cannot create a GL device context.", ELL_ERROR);
        return false;
    }

    // search for pixel format the simple way
    if (pixel_format == 0 || (!SetPixelFormat(hdc_, pixel_format, &pfd)))
    {
        for (u32 i = 0; i<5; ++i)
        {
            if (i == 1)
            {
                if (params_.stencil_buffer_)
                {
                    //os::Printer::log("Cannot create a GL device with stencil buffer, disabling stencil shadows.", ELL_WARNING);
                    params_.stencil_buffer_ = false;
                    pfd.cStencilBits = 0;
                }
                else
                    continue;
            }
            else if (i == 2)
            {
                pfd.cDepthBits = 24;
            }
            else if (i == 3)
            {
                if (params_.color_bits_ != 16)
                    pfd.cColorBits = 16;
                else
                    continue;
            }
            else if (i == 4)
                {
                    //os::Printer::log("Cannot create a GL device context", "No suitable format.", ELL_ERROR);
                    return false;
                }

            // choose pixelformat
            pixel_format = ChoosePixelFormat(hdc_, &pfd);
            if (pixel_format)
                break;
        }

        // set pixel format
        if (!SetPixelFormat(hdc_, pixel_format, &pfd))
        {
            //os::Printer::log("Cannot set the pixel format.", ELL_ERROR);
            return false;
        }
    }

    hrc_ = wglCreateContext(hdc_);
    if (!hrc_)
    {
        //os::Printer::log("Cannot create a GL rendering context.", ELL_ERROR);
        return false;
    }


    return true;
}

void kong::video::COpenGLDriver::SetMaterial(const SMaterial& material)
{

}

bool kong::video::COpenGLDriver::BeginScene(bool back_buffer, bool z_buffer, SColor color)
{
    ClearBuffers(back_buffer, z_buffer, false, color);
}

bool kong::video::COpenGLDriver::EndScene()
{
    glFlush();

#ifdef _KONG_COMPILE_WITH_WINDOWS_DEVICE_
    return SwapBuffers(hdc_) == TRUE;
#endif
}

void kong::video::COpenGLDriver::SetTransform(E_TRANSFORMATION_STATE state, const core::Matrixf& mat)
{
}

void kong::video::COpenGLDriver::ClearBuffers(bool back_buffer, bool z_buffer, bool stencil_buffer, SColor color)
{
    GLbitfield mask = 0;

    if (back_buffer)
    {
        const f32 inv = 1.f / 255.f;
        glClearColor(color.GetRed() * inv, color.GetGreen() * inv, color.GetBlue() * inv, color.GetAlpha() * inv);
    }

    if (z_buffer)
    {
        glDepthMask(GL_TRUE);
        mask |= GL_DEPTH_BUFFER_BIT;
    }

    if (stencil_buffer)
        mask |= GL_STENCIL_BUFFER_BIT;

    if (mask)
        glClear(mask);
}

namespace kong
{
    namespace video
    {
#ifdef _KONG_COMPILE_WITH_OPENGL_
        IVideoDriver* CreateOpenGLDriver(const kong::SKongCreationParameters& params,
            /*io::IFileSystem* io,*/ CKongDeviceWin32* device)
        {
            COpenGLDriver *driver = new COpenGLDriver(params, device);
#ifdef _KONG_COMPILE_WITH_OPENGL_
            if (!driver->InitDriver(device))
            {
                return nullptr;
            }
            return driver;
#else
            return nullptr;
#endif
        }
#endif
    }
}