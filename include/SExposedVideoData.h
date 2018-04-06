// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _SEXPOSEDVIDEODATA_H_
#define _SEXPOSEDVIDEODATA_H_

// forward declarations for internal pointers
struct IDirect3D9;
struct IDirect3DDevice9;
struct IDirect3D8;
struct IDirect3DDevice8;

namespace kong
{
    namespace video
    {

        //! structure for holding data describing a driver and operating system specific data.
        /** This data can be retrived by IVideoDriver::getExposedVideoData(). Use this with caution.
        This only should be used to make it possible to extend the engine easily without
        modification of its source. Note that this structure does not contain any valid data, if
        you are using the software or the null device.
        */
        struct SExposedVideoData
        {
            SExposedVideoData()
            {
                OpenGLWin32.hdc_ = nullptr; 
                OpenGLWin32.hrc_ = nullptr;
                OpenGLWin32.hwnd_ = nullptr;
            }
            explicit SExposedVideoData(void* window)
            {
                OpenGLWin32.hdc_ = nullptr;
                OpenGLWin32.hrc_ = nullptr; 
                OpenGLWin32.hwnd_ = window;
            }

            union
            {
                struct
                {
                    //! Pointer to the IDirect3D9 interface
                    IDirect3D9* d3d9_;

                    //! Pointer to the IDirect3DDevice9 interface
                    IDirect3DDevice9* d3ddev9_;

                    //! Window handle.
                    /** Get with for example HWND h = reinterpret_cast<HWND>(exposedData.D3D9.HWnd) */
                    void* hwnd_;

                } D3D9;

                struct
                {
                    //! Pointer to the IDirect3D8 interface
                    IDirect3D8* d3d8_;

                    //! Pointer to the IDirect3DDevice8 interface
                    IDirect3DDevice8* d3ddev8_;

                    //! Window handle.
                    /** Get with for example with: HWND h = reinterpret_cast<HWND>(exposedData.D3D8.HWnd) */
                    void* hwnd_;

                } D3D8;

                struct
                {
                    //! Private GDI Device Context.
                    /** Get if for example with: HDC h = reinterpret_cast<HDC>(exposedData.OpenGLWin32.HDc) */
                    void* hdc_;

                    //! Permanent Rendering Context.
                    /** Get if for example with: HGLRC h = reinterpret_cast<HGLRC>(exposedData.OpenGLWin32.HRc) */
                    void* hrc_;

                    //! Window handle.
                    /** Get with for example with: HWND h = reinterpret_cast<HWND>(exposedData.OpenGLWin32.HWnd) */
                    void* hwnd_;
                } OpenGLWin32;

                struct
                {
                    // XWindow handles
                    void* x11display_;
                    void* x11context_;
                    unsigned long x11window_;
                } OpenGLLinux;
            };
        };

    } // end namespace video
} // end namespace kong

#endif