// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _CPERSPECTIVECAMERASCENENODE_H_
#define _CPERSPECTIVECAMERASCENENODE_H_

#include "ICameraSceneNode.h"

namespace kong
{
    namespace scene
    {
        class CPerspectiveCameraSceneNode : public ICameraSceneNode
        {
        public:
            CPerspectiveCameraSceneNode(ISceneNode *parent, ISceneManager * mgr, s32 id,
                f32 fovy = 90.f, f32 aspect = 1.33f, f32 zn = 0.1f, f32 zf = 50.f);
            CPerspectiveCameraSceneNode(ISceneNode *parent, ISceneManager * mgr, s32 id,
                core::Vector3Df eye, core::Vector3Df up, core::Vector3Df at,
                f32 fovy = 90.f, f32 aspect = 1.33f, f32 zn = 0.1f, f32 zf = 50.f);

            void SetValues(f32 fovy = 90.f, f32 aspect = 1.78f, f32 zn = 1.f, f32 zf = 50.f);
            void UpdateProjectTransform() override;
            int GetCameraType() override;

            //void Render() override;

        private:
            f32 fovy_;
            f32 aspect_;
        };
    }
}

#endif