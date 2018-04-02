// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _CSCENEMANAGER_H_
#define _CSCENEMANAGER_H_
#include "ISceneManager.h"
#include "Array.h"

namespace kong
{
    namespace scene
    {
        class CSceneManager : public ISceneManager
        {
        private:
            struct DefaultNodeEntry
            {
                DefaultNodeEntry(ISceneNode* n) :
                    Node(n), TextureValue(0)
                {
                    if (n->GetMaterialCount())
                        TextureValue = (n->GetMaterial(0).GetTexture(0));
                }

                bool operator < (const DefaultNodeEntry& other) const
                {
                    return (TextureValue < other.TextureValue);
                }

                ISceneNode* Node;
            private:
                void* TextureValue;
            };

            core::Array<ISceneNode *> camera_list_;
            core::Array<ISceneNode *> light_list_;
            core::Array<ISceneNode *> shadow_node_list_;
        };
    }
}

#endif