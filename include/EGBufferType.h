// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _EGBUFFERTYPE_H_
#define _EGBUFFERTYPE_H_

#include "KongTypes.h"

namespace kong
{
    namespace video
    {
        enum E_GBUFFER_TYPE
        {
            EGBT_POSTION,
            EGBT_NORMAL,
            EGBT_DIFFUSE,
            EGBT_COUNT
        };

        const c8 *const gbuffer_type_name[] =
        {
            "position_tex",
            "normal_tex",
            "diffuse_tex",
            nullptr
        };
    }
}

#endif