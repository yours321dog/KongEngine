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
        : create_params_(param), close_(false), video_driver_(nullptr), scene_manager_(nullptr), user_receiver_(param.event_receiver_)
    {
    }

    kong::CKongDeviceStub::~CKongDeviceStub()
    {
    }

    void kong::CKongDeviceStub::CreateScene()
    {
        scene_manager_ = scene::CreateSceneManager(video_driver_);
    }

    bool CKongDeviceStub::PostEventFromUser(const SEvent &post_event) const
    {
        bool absorbed = false;
        if (user_receiver_ != nullptr)
        {
            absorbed = user_receiver_->OnEvent(post_event);
        }

        return absorbed;
    }
}


