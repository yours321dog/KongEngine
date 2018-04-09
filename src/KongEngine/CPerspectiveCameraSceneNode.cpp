// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "CPerspectiveCameraSceneNode.h"
#include "KongMath.h"

namespace kong
{
    namespace scene
    {
        CPerspectiveCameraSceneNode::CPerspectiveCameraSceneNode(ISceneNode *parent, ISceneManager * mgr, s32 id, f32 fovy, f32 aspect, f32 zn, f32 zf)
            : ICameraSceneNode(parent, mgr, id, core::Vector3Df(0, 0, -1), core::Vector3Df(0, 1, 0), core::Vector3Df(0, 0, 0), zn, zf),
            fovy_(fovy), aspect_(aspect)
        {
        }

        CPerspectiveCameraSceneNode::CPerspectiveCameraSceneNode(ISceneNode *parent, ISceneManager * mgr, s32 id, core::Vector3Df eye, core::Vector3Df up,
            core::Vector3Df at, f32 fovy, f32 aspect, f32 zn, f32 zf)
            : ICameraSceneNode(parent, mgr, id, eye, up, at, zn, zf), fovy_(fovy), aspect_(aspect)
        {
        }

        void CPerspectiveCameraSceneNode::SetValues(f32 fovy, f32 aspect, f32 zn, f32 zf)
        {
            fovy_ = fovy;
            aspect_ = aspect;
            zn_ = zn;
            zf_ = zf;
        }

        void CPerspectiveCameraSceneNode::UpdateProjectTransform()
        {
            project_.Zero();
            float fax = 1.f / tan(fovy_ / 180.f * core::PI / 2.f);
            project_(0, 0) = fax / aspect_;
            project_(1, 1) = fax;
            //project_(2, 2) = zf_ / (zf_ - zn_);
            //project_(3, 2) = zf_ * zn_ / (zn_ - zf_);
            //project_(2, 3) = 1;

            project_(2, 2) = (zf_ + zn_) / (zf_ - zn_);
            project_(3, 2) = 2.f * zf_ * zn_ / (zn_ - zf_);
            project_(2, 3) = 1;
        }

        int CPerspectiveCameraSceneNode::GetCameraType()
        {
            return PERSPECTIVE;
        }

        //void CPerspectiveCameraSceneNode::Render()
        //{
        //}
    }
}