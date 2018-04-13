// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "KongTypes.h"

#ifdef _KONG_WINDOWS_
#include <windows.h>
#if defined(_DEBUG) && !defined(__GNUWIN32__) && !defined(_WIN32_WCE)
#include <crtdbg.h>
#endif // _DEBUG
#endif

#include "KongEngine.h"

#ifdef _KONG_COMPILE_WITH_WINDOWS_DEVICE_
#include "CKongDeviceWin32.h"
#endif

namespace kong
{
    KONG_API KongDevice* KONGCALLCONV CreateDevice(
        const core::Dimension2d<u32>& window_size,
        u32 bits,
        bool fullscreen,
        bool stencilbuffer,
        bool vsync,
        IEventReceiver *receiver)
    {
        SKongCreationParameters param;
        param.window_size_ = window_size;
        param.event_receiver_ = receiver;

        return CreateDeviceEx(param);
    }

    KONG_API KongDevice* KONGCALLCONV CreateDeviceEx(
        const SKongCreationParameters& params)
    {
        KongDevice *dev = nullptr;

#ifdef _KONG_COMPILE_WITH_WINDOWS_DEVICE_
        //if (params.DeviceType == EIDT_WIN32 || (!dev && params.DeviceType == EIDT_BEST))
        dev = new CKongDeviceWin32(params);
#endif

        return dev;
    }

    namespace core
    {
        const Matrixf identity_matrix(Matrixf::IDENTITY);
        kong::core::stringc LOCALE_DECIMAL_POINTS(".");
    }

    namespace video
    {
        SMaterial IdentityMaterial;
    }
} // end namespace kong