// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _C_KONG_DEVICE_STUB_H_
#define _C_KONG_DEVICE_STUB_H_
#include "KongDevice.h"
#include "SKongCreationParameters.h"
#include "IVideoDriver.h"
#include "IEventReceiver.h"
#include "IFileSystem.h"

namespace kong
{
    namespace io
    {
        IFileSystem* CreateFileSystem();
    }

    namespace video
    {
        //IVideoDriver *CreateNullDriver(io::IFileSystem *io, const core::Vector2D<u32> &screen_size);
    }
    class CKongDeviceStub : public KongDevice
    {
    public:
        CKongDeviceStub(const SKongCreationParameters &param);

        virtual ~CKongDeviceStub();

        // handle event
        bool PostEventFromUser(const SEvent &post_event) const;

    protected:
        void CreateScene();

        SKongCreationParameters create_params_;
        bool close_;

        video::IVideoDriver *video_driver_;
        scene::ISceneManager* scene_manager_;
        IEventReceiver *user_receiver_;

        io::IFileSystem *file_system_;
    };

} // end namespace kong

#endif