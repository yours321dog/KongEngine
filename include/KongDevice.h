// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _I_KONG_DEVICE_H_
#define _I_KONG_DEVICE_H_

namespace kong
{
    class KongDevice
    {
    public :
        virtual ~KongDevice() {}

        virtual bool run() = 0;
    };
} // end namespace kong

#endif