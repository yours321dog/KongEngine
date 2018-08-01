// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _DEFAULTNODEENTRY_H_
#define _DEFAULTNODEENTRY_H_

#include "ISceneNode.h"

namespace kong
{
    namespace scene
    {
        struct DefaultNodeEntry
        {
            DefaultNodeEntry(ISceneNode* n = nullptr) :
            node_(n), texture_value_(nullptr)
            {
                //if (n->GetMaterialCount())
                //    TextureValue = (n->GetMaterial(0).GetTexture(0));
            }

            DefaultNodeEntry(const DefaultNodeEntry &other)
            {
                node_ = other.node_;
                texture_value_ = other.texture_value_;
            }

            ~DefaultNodeEntry()
            {
                //delete node_;
            }

            bool operator < (const DefaultNodeEntry& other) const
            {
                return (texture_value_ < other.texture_value_);
            }

            ISceneNode* node_;
        private:
            void* texture_value_;
        };
    }
}

#endif