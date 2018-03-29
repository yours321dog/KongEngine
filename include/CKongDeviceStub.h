// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _C_KONG_DEVICE_STUB_H_
#define _C_KONG_DEVICE_STUB_H_
#include "KongDevice.h"
#include "SKongCreationParameters.h"

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
        SKongCreationParameters CreationParams;
        bool close_;
    };

} // end namespace kong

#endif