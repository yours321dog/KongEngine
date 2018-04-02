// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _CORTHOGONALCAMERASCENENODE_H_
#define _CORTHOGONALCAMERASCENENODE_H_

#include "ICameraSceneNode.h"
namespace kong
{
    namespace scene
    {
        class COrthogonalCameraSceneNode : public kong::scene::ICameraSceneNode
        {
        public:
            COrthogonalCameraSceneNode(f32 height = 1.0, f32 aspect = 1.78f, f32 zn = 1.f, f32 zf = 200.f);
            COrthogonalCameraSceneNode(core::Vector3Df eye, core::Vector3Df up, core::Vector3Df at,
                f32 height = 1.f, f32 aspect = 1.78f, f32 zn = 1.f, f32 zf = 200.f);
            ~COrthogonalCameraSceneNode() = default;
            void SetValues(f32 height = 120.f, f32 aspect = 1.78f, f32 zn = 1.f, f32 zf = 200.f);
            void UpdateProjectTransform() override;
            int GetCameraType() override;

        private:
            f32 height_;
            f32 aspect_;
        };
    } // end namespace scene
} // end namespace kong


#endif