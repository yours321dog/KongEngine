// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _C_KONG_DEVICE_WIN32_H_
#define _C_KONG_DEVICE_WIN32_H_

#include "KongCompileConfig.h"

#include "CKongDeviceStub.h"
#include "SKongCreationParameters.h"

#define WIN32_LEAN_AND_MEAN
#if !defined(_IRR_XBOX_PLATFORM_)
#include <windows.h>
#include <mmsystem.h> // For JOYCAPS
#include <windowsx.h>
#endif
#if !defined(GET_X_LPARAM)
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

namespace kong
{
    class CKongDeviceWin32 : public CKongDeviceStub
    {
    public:
        CKongDeviceWin32(const SKongCreationParameters &param);

        virtual ~CKongDeviceWin32();

        virtual bool run();

        virtual video::IVideoDriver* GetVideoDriver();

        //! Provides access to the scene manager.
        scene::ISceneManager* GetSceneManager() override;

        void ScreenClose();

        void ScreenDispatch();

    protected:
        //! create the driver
        void CreateVideo();

        int screen_w, screen_h;
        int screen_mx, screen_my, screen_mb;
        HWND screen_handle;
        HDC screen_dc;
        HBITMAP screen_hb;
        HBITMAP screen_ob;
        unsigned char *screen_fb;
        long screen_pitch;
    };
}

#endif