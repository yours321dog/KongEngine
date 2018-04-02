// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _CSCENEMANAGER_H_
#define _CSCENEMANAGER_H_
#include "ISceneManager.h"
#include "Array.h"
#include "ISceneNode.h"
#include "ICameraSceneNode.h"
#include "IVideoDriver.h"

namespace kong
{
    namespace scene
    {
        class CSceneManager : public ISceneManager, public ISceneNode
        {
        public:
            //! constructor
            CSceneManager(video::IVideoDriver* driver/*, io::IFileSystem* fs,
                gui::ICursorControl* cursorControl, IMeshCache* cache = 0,
                gui::IGUIEnvironment *guiEnvironment = 0*/);

            //! destructor
            virtual ~CSceneManager();

        private:
            struct DefaultNodeEntry
            {
                DefaultNodeEntry(ISceneNode* n) :
                    node_(n), texture_value_(nullptr)
                {
                    //if (n->GetMaterialCount())
                    //    TextureValue = (n->GetMaterial(0).GetTexture(0));
                }

                bool operator < (const DefaultNodeEntry& other) const
                {
                    return (texture_value_ < other.texture_value_);
                }

                ISceneNode* node_;
            private:
                void* texture_value_;
            };

            //! video driver
            video::IVideoDriver* driver_;

            core::Array<ISceneNode *> camera_list_;
            core::Array<ISceneNode *> light_list_;
            core::Array<ISceneNode *> shadow_node_list_;
            core::Array<DefaultNodeEntry> solid_node_list_;

            video::SColor shadow_color_;
            video::SColor ambient_light_;

            ICameraSceneNode *active_camera_;
            core::Vector3Df cam_world_pos_;
        };
    }
}

#endif