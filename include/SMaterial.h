// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _SMATERIAL_H_
#define _SMATERIAL_H_
#include "SColor.h"

namespace kong
{
    namespace video
    {
        class SMaterial
        {
            SColor ambient_color_;
            SColor diffuse_color_;
            SColor specular_color_;
            SColor emissive_color_;

            f32 shininess_;

            bool use_mip_maps_ : 1;
        };

        //! global const identity Material
        KONG_API extern SMaterial IdentityMaterial;
    } // end namespace video
} // end namespace kong

#endif