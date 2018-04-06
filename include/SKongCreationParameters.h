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
            window_size_(core::Dimension2d<u32>(800, 600)), fullscreen_(false), color_bits_(24), z_buffer_bits_(32), stencil_buffer_(true), window_id_(nullptr)
        {
        }

        SKongCreationParameters(const SKongCreationParameters &other)
        {
            window_size_ = other.window_size_;
            fullscreen_ = other.fullscreen_;
            color_bits_ = other.color_bits_;
            z_buffer_bits_ = other.z_buffer_bits_;
            stencil_buffer_ = other.stencil_buffer_;
            window_id_ = other.window_id_;
        }

        SKongCreationParameters &operator=(const SKongCreationParameters &other)
        {
            window_size_ = other.window_size_;
            fullscreen_ = other.fullscreen_;
            color_bits_ = other.color_bits_;
            z_buffer_bits_ = other.z_buffer_bits_;
            stencil_buffer_ = other.stencil_buffer_;
            window_id_ = other.window_id_;
            return *this;
        }

        core::Dimension2d<u32> window_size_;

        //! Should be set to true if the device should run in fullscreen.
        /** Otherwise the device runs in windowed mode. Default: false. */
        bool fullscreen_;

        // color bits
        u32 color_bits_;

        // z buffer bits
        u32 z_buffer_bits_;

        // whether use stencil buffer or not
        bool stencil_buffer_;

        // window id
        void *window_id_;
    };
}

#endif