// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _KONG_ENGINE_H_
#define _KONG_ENGINE_H_

#include "KongCompileConfig.h"
#include "KongDevice.h"
#include "KongTypes.h"
#include "Dimension2d.h"
#include "SKongCreationParameters.h"

namespace kong
{
    extern "C" KONG_API KongDevice* KONGCALLCONV CreateDevice(
        // parantheses are necessary for some compilers
        const core::Dimension2d<u32>& windowSize = (core::Dimension2d<u32>(640, 480)),
        u32 bits = 16,
        bool fullscreen = false,
        bool stencilbuffer = false,
        bool vsync = false);

    extern "C" KONG_API KongDevice* KONGCALLCONV CreateDeviceEx(
        const SKongCreationParameters& parameters);
}

#endif