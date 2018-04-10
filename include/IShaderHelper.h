// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _ISHADERHELPTER_H_
#define _ISHADERHELPTER_H_

#include <string>
#include "KongTypes.h"

namespace kong
{
    namespace video
    {
        class IShaderHelper
        {
        public:
            virtual ~IShaderHelper() = default;

            // activate the shader
            virtual void Use() = 0;

            // uniform set functions
            virtual void SetBool(const std::string &name, bool value) const = 0;
            virtual void SetInt(const std::string &name, s32 value) const = 0;
            virtual void SetFloat(const std::string &name, f32 value) const = 0;
        };
    } // end namespace video
} // end namespace kong

#endif