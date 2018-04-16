// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _ICAMERASCENENODE_H_
#define _ICAMERASCENENODE_H_
#include "Vector.h"
#include "Matrix.h"

#include "ISceneNode.h"
#include "ISceneManager.h"

namespace kong
{
    namespace scene
    {
        class ICameraSceneNode : public ISceneNode
        {
        public:
            ICameraSceneNode(ISceneNode *parent, ISceneManager * mgr, s32 id = -1);
            ICameraSceneNode(ISceneNode *parent, ISceneManager * mgr, s32 id, const core::Vector3Df& eye, const core::Vector3Df& up,
                const core::Vector3Df& at, f32 zn = 1.f, f32 zf = 100.f);
            virtual ~ICameraSceneNode();

            void UpdateViewTransform();
            core::Matrixf GetViewTransform();
            core::Matrixf GetProjectTransform();
            void LookAt(const core::Vector3Df &at);
            void SetUp(const core::Vector3Df &up);
            void SetEye(const core::Vector3Df &eye);
            virtual void UpdateProjectTransform() = 0;
            core::Vector3Df ToEye(const core::Vector3Df &pos) const;
            core::Vector3Df GetEye() const;
            core::Vector3Df GetLookAt() const;
            f32 GetZn() const;

            virtual int GetCameraType() = 0;

            // register node
            void OnRegisterSceneNode() override;

            // render camera node
            void Render() override;

            const core::aabbox3d<f32>& GetBoundingBox() const override;

            enum CAMERA_TYPE
            {
                ORTHOGONAL,
                PERSPECTIVE,
                OTHER
            };

        protected:
            core::Matrixf project_;
            f32 zn_;
            f32 zf_;

        private:
            core::Matrixf view_;
            core::Vector3Df eye_;
            core::Vector3Df up_;
            core::Vector3Df at_;
            core::Vector3Df right_;
            core::Vector3Df to_;
        };

        inline ICameraSceneNode::ICameraSceneNode(ISceneNode *parent, ISceneManager * mgr, s32 id)
            : ISceneNode(parent, mgr, id), zn_(0), zf_(0), eye_(0, 0, 0, 1), up_(0, 1, 0, 1),
            at_(0, 0, 1, 1), right_(1, 0, 0, 1), to_(0, 0, 1, 1)
        {
            view_.Identity();
        }

        inline ICameraSceneNode::ICameraSceneNode(ISceneNode *parent, ISceneManager * mgr, s32 id, 
            const core::Vector3Df& eye, const core::Vector3Df& up,
            const core::Vector3Df &at, f32 zn, f32 zf)
            : ISceneNode(parent, mgr, id), zn_(zn), zf_(zf), eye_(eye), up_(up), at_(at)
        {
            up_.Normalize();
            to_ = at_ - eye_;
            to_.Normalize();
            right_.CrossProduct(up_, to_);
            view_.Identity();
        }

        inline ICameraSceneNode::~ICameraSceneNode()
        = default;

        inline void ICameraSceneNode::UpdateViewTransform()
        {
            to_ = at_ - eye_;
            to_.Normalize();
            right_.CrossProduct(up_, to_);
            right_.Normalize();
            up_.CrossProduct(to_, right_);

            view_.Identity();

            view_(0, 0) = right_(0);
            view_(1, 0) = right_(1);
            view_(2, 0) = right_(2);
            view_(3, 0) = -(right_.DotProduct(eye_));

            view_(0, 1) = up_(0);
            view_(1, 1) = up_(1);
            view_(2, 1) = up_(2);
            view_(3, 1) = -(up_.DotProduct(eye_));

            view_(0, 2) = to_(0);
            view_(1, 2) = to_(1);
            view_(2, 2) = to_(2);
            view_(3, 2) = -(to_.DotProduct(eye_));
        }

        inline core::Matrixf ICameraSceneNode::GetViewTransform()
        {
            UpdateViewTransform();
            return view_;
        }

        inline core::Matrixf ICameraSceneNode::GetProjectTransform()
        {
            UpdateProjectTransform();
            return project_;
        }

        inline void ICameraSceneNode::LookAt(const core::Vector3Df& at)
        {
            at_ = at;
        }

        inline void ICameraSceneNode::SetUp(const core::Vector3Df& up)
        {
            up_ = up;
            up_.Normalize();
        }

        inline void ICameraSceneNode::SetEye(const core::Vector3Df& eye)
        {
            eye_ = eye;
        }

        inline core::Vector3Df ICameraSceneNode::ToEye(const core::Vector3Df& pos) const
        {
            return eye_ - pos;
        }

        inline core::Vector3Df ICameraSceneNode::GetEye() const
        {
            return eye_;
        }

        inline core::Vector3Df ICameraSceneNode::GetLookAt() const
        {
            return at_;
        }

        inline f32 ICameraSceneNode::GetZn() const
        {
            return zn_;
        }

        inline void ICameraSceneNode::OnRegisterSceneNode()
        {
            if (scene_manager_->GetActiveCamera() == this)
                scene_manager_->RegisterNodeForRendering(this, ESNRP_CAMERA);

            ISceneNode::OnRegisterSceneNode();
        }

        inline void ICameraSceneNode::Render()
        {
            UpdateViewTransform();
            UpdateProjectTransform();

            video::IVideoDriver *driver = scene_manager_->GetVideoDriver();

            if (driver != nullptr)
            {
                driver->SetTransform(video::ETS_PROJECTION, project_);
                driver->SetTransform(video::ETS_VIEW, view_);
            }
        }

        inline const core::aabbox3d<f32>& ICameraSceneNode::GetBoundingBox() const
        {
            _KONG_DEBUG_BREAK_IF(true) // Bounding Box of Scene Manager wanted.

            // should never be used.
            return *(static_cast<core::aabbox3d<f32>*>(nullptr));
        }
    }
}

#endif