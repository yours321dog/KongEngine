// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "COrthogonalCameraSceneNode.h"

namespace kong
{
    namespace scene
    {
        COrthogonalCameraSceneNode::COrthogonalCameraSceneNode(ISceneNode *parent, ISceneManager * mgr, s32 id, f32 height, f32 aspect, f32 zn, f32 zf)
            :ICameraSceneNode(parent, mgr, id, core::Vector3Df(0, 0, -1), core::Vector3Df(0, 1, 0), kong::core::Vector3Df(0, 0, 0), zn, zf),
            height_(height), aspect_(aspect)
        {
        }

        COrthogonalCameraSceneNode::COrthogonalCameraSceneNode(ISceneNode *parent, ISceneManager * mgr, s32 id, core::Vector3Df eye, core::Vector3Df up,
            core::Vector3Df at, f32 height, f32 aspect, f32 zn, f32 zf)
            : ICameraSceneNode(parent, mgr, id, eye, up, at, zn, zf),
            height_(height), aspect_(aspect)
        {
        }

        void COrthogonalCameraSceneNode::SetValues(f32 height, f32 aspect, f32 zn, f32 zf)
        {
            height_ = height;
            aspect_ = aspect;
            zn_ = zn;
            zf_ = zf;
        }

        void COrthogonalCameraSceneNode::UpdateProjectTransform()
        {
            project_.Zero();
            const float fax = 2.f / height_;
            project_(0, 0) = fax / aspect_;
            project_(1, 1) = fax;
            project_(2, 2) = 1 / (zf_ - zn_);
            project_(3, 2) = (zn_) / (zn_ - zf_);
            project_(3, 3) = 1;
        }

        int COrthogonalCameraSceneNode::GetCameraType()
        {
            return ECT_ORTHOGONAL;
        }
    } // end namespace scene
} //end namespace kong