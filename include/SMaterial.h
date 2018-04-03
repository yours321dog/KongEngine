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
        public:
            bool operator!=(const SMaterial &other) const;
            bool operator==(const SMaterial &other) const;

            SColor ambient_color_;
            SColor diffuse_color_;
            SColor specular_color_;
            SColor emissive_color_;

            f32 shininess_;

            bool use_mip_maps_ : 1;
        };

        inline bool SMaterial::operator!=(const SMaterial& other) const
        {
            const bool different = ambient_color_ != other.ambient_color_ ||
                diffuse_color_ != other.diffuse_color_ ||
                specular_color_ != other.specular_color_ ||
                emissive_color_ != other.emissive_color_ ||
                shininess_ != other.shininess_ ||
                use_mip_maps_ != other.use_mip_maps_;
            return different;
        }

        inline bool SMaterial::operator==(const SMaterial& other) const
        {
            return !(*this != other);
        }

        //! global const identity Material
        KONG_API extern SMaterial IdentityMaterial;
    } // end namespace video
} // end namespace kong

#endif