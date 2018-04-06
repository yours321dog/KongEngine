// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _C_KONG_DEVICE_STUB_H_
#define _C_KONG_DEVICE_STUB_H_
#include "KongDevice.h"
#include "SKongCreationParameters.h"
#include "IVideoDriver.h"

namespace kong
{
    namespace video
    {
        //IVideoDriver *CreateNullDriver(io::IFileSystem *io, const core::Vector2D<u32> &screen_size);
    }
    class CKongDeviceStub : public KongDevice
    {
    public:
        CKongDeviceStub(const SKongCreationParameters &param);

        virtual ~CKongDeviceStub();

    protected:
        SKongCreationParameters create_params_;
        bool close_;

        video::IVideoDriver *video_driver_;
    };

} // end namespace kong

#endif