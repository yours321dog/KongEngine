// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _I_VIDEO_DRIVER_H_
#define _I_VIDEO_DRIVER_H_
#include "SColor.h"

namespace kong
{
    namespace video
    {
        class IVideoDriver
        {
        public:
            virtual bool beginScene(bool back_buffer = true, bool z_buffer = true, SColor color = SColor(255, 0, 0, 0));
        };
    }
}

#endif