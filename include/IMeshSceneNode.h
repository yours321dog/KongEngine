// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _IMESHSCENENODE_H_
#define _IMESHSCENENODE_H_

#include "ISceneNode.h"

namespace kong
{
    namespace scene
    {
        class IMeshSceneNode : public ISceneNode
        {
            //! Constructor
            /** Use setMesh() to set the mesh to display.
            */
            IMeshSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
            const core::Vector3Df& position = core::Vector3Df(0, 0, 0),
            const core::Vector3Df& rotation = core::Vector3Df(0, 0, 0),
            const core::Vector3Df& scale = core::Vector3Df(1, 1, 1))
            : ISceneNode(parent, mgr, id, position, rotation, scale) {}


        };
    }
}

#endif