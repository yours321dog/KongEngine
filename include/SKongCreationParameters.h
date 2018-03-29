// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _S_KONG_CREATION_PARAMETERS_H_
#define _S_KONG_CREATION_PARAMETERS_H_

#include "KongTypes.h"
#include "Dimension2d.h"

namespace kong
{
    class SKongCreationParameters
    {
    public:
        SKongCreationParameters() :
            window_size_(core::Dimension2d<u32>(800, 600)) {}

        SKongCreationParameters(const SKongCreationParameters &other)
        {
            window_size_ = other.window_size_;
        }

        SKongCreationParameters &operator=(const SKongCreationParameters &other)
        {
            window_size_ = other.window_size_;
            return *this;
        }

        core::Dimension2d<u32> window_size_;
    };
}

#endif