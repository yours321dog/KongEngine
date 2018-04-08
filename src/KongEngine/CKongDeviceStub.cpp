// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "CKongDeviceStub.h"
//#include "IVideoDriver.h"

namespace kong
{
    namespace scene
    {
        ISceneManager* CreateSceneManager(video::IVideoDriver* driver
            /*io::IFileSystem* fs, gui::ICursorControl* cc, gui::IGUIEnvironment *gui*/);
    }

    kong::CKongDeviceStub::CKongDeviceStub(const SKongCreationParameters &param)
        : create_params_(param), close_(false)
    {
    }

    kong::CKongDeviceStub::~CKongDeviceStub()
    {
    }

    void kong::CKongDeviceStub::CreateScene()
    {
        scene_manager_ = scene::CreateSceneManager(video_driver_);
    }
}


