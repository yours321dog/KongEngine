// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _I_KONG_DEVICE_H_
#define _I_KONG_DEVICE_H_
#include "IVideoDriver.h"

namespace kong
{
    namespace scene {
        class ISceneManager;
    } // end namespace scene

    class KongDevice
    {
    public :
        virtual ~KongDevice() {}

        virtual bool run() = 0;

        //! Provides access to the video driver for drawing 3d and 2d geometry.
        /** \return Pointer the video driver. */
        virtual video::IVideoDriver* GetVideoDriver() = 0;

        //! Provides access to the scene manager.
        /** \return Pointer to the scene manager. */
        virtual scene::ISceneManager* GetSceneManager() = 0;
    };
} // end namespace kong

#endif