// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "ISceneManager.h"
#include "CLightSceneNode.h"
#include "COrthogonalCameraSceneNode.h"
#include "CPerspectiveCameraSceneNode.h"

namespace kong
{
    namespace scene
    {
        CLightSceneNode::CLightSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
            const core::vector3df& position, video::SColorf& color, f32 radius)
            : ILightSceneNode(parent, mgr, id, position), driver_light_index_(-1), light_is_on_(true), camera_(nullptr)
        {
            light_data_.diffuse_color_ = color;
            // set some useful specular color
            light_data_.specular_color_ = color.GetInterpolated(video::SColor(255, 255, 255, 255), 0.7f);
            light_data_.position_ = position;

            //CLightSceneNode::SetRadius(radius);
            //SetRadius(radius);
            light_data_.radius_ = radius;
            light_data_.attenuation_.Set(1.f, 1.f / radius, 0.f);
            DoLightRecalc();

            // Init camera type
            ResetCamera();
        }

        CLightSceneNode::~CLightSceneNode()
        {
            delete camera_;
        }

        void CLightSceneNode::OnRegisterSceneNode()
        {
            DoLightRecalc();

            if (is_visible_)
                scene_manager_->RegisterNodeForRendering(this, ESNRP_LIGHT);

            ISceneNode::OnRegisterSceneNode();
        }

        void CLightSceneNode::Render()
        {
            video::IVideoDriver* driver = scene_manager_->GetVideoDriver();
            if (!driver)
                return;

            driver_light_index_ = driver->AddDynamicLight(light_data_);
            SetVisible(light_is_on_);

            driver->SetTransform(video::ETS_LIGHT_PROJECTION, camera_->GetProjectTransform());
            driver->SetTransform(video::ETS_LIGHT_VIEW, camera_->GetViewTransform());
        }

        const core::aabbox3d<f32>& CLightSceneNode::GetBoundingBox() const
        {
            return box_;
        }

        void CLightSceneNode::SetLightData(const video::SLight& light)
        {
            const bool delete_camera = light.type_ != light_data_.type_;
            light_data_ = light;
            ResetCamera(delete_camera);
        }

        const video::SLight& CLightSceneNode::GetLightData() const
        {
            return light_data_;
        }

        video::SLight& CLightSceneNode::GetLightData()
        {
            return light_data_;
        }

        void CLightSceneNode::SetVisible(bool isVisible)
        {
            is_visible_ = isVisible;
        }

        void CLightSceneNode::SetLightType(video::E_LIGHT_TYPE type)
        {
            light_data_.type_ = type;
        }

        video::E_LIGHT_TYPE CLightSceneNode::GetLightType() const
        {
            return light_data_.type_;
        }

        void CLightSceneNode::SetRadius(f32 radius)
        {
            light_data_.radius_ = radius;
            light_data_.attenuation_.Set(1.f, 1.f / radius, 0.f);
            DoLightRecalc();
        }

        f32 CLightSceneNode::GetRadius() const
        {
            return light_data_.radius_;
        }

        core::Matrixf CLightSceneNode::GetLightTransform() const
        {
            core::vector3df to = light_data_.direction_;
            to.Normalize();
            core::vector3df up = to + core::vector3df(0.f, 1.f, 0.f);
            core::vector3df right;
            right.CrossProduct(up, to);
            right.Normalize();
            up.CrossProduct(to, right);

            core::Matrixf res;

            res(0, 0) = right(0);
            res(1, 0) = right(1);
            res(2, 0) = right(2);
            res(3, 0) = -(right.DotProduct(light_data_.position_));

            res(0, 1) = up(0);
            res(1, 1) = up(1);
            res(2, 1) = up(2);
            res(3, 1) = -(up.DotProduct(light_data_.position_));

            res(0, 2) = to(0);
            res(1, 2) = to(1);
            res(2, 2) = to(2);
            res(3, 2) = -(to.DotProduct(light_data_.position_));

            return camera_->GetProjectTransform();
        }

        void CLightSceneNode::RenderShadow()
        {
            video::IVideoDriver* driver = scene_manager_->GetVideoDriver();
            if (!driver)
                return;

            driver->SetTransform(video::ETS_VIEW, camera_->GetViewTransform());
            driver->SetTransform(video::ETS_PROJECTION, camera_->GetProjectTransform());

            //driver_light_index_ = driver->AddDynamicLight(light_data_);
        }

        void CLightSceneNode::RecalculateLightBoundingBox(core::aabbox3df &box)
        {
            core::vector3df corners[8];

            corners[0] = core::vector3df(box.MinEdge.x_, box.MinEdge.y_, box.MinEdge.z_);
            corners[1] = core::vector3df(box.MinEdge.x_, box.MinEdge.y_, box.MaxEdge.z_);
            corners[2] = core::vector3df(box.MinEdge.x_, box.MaxEdge.y_, box.MinEdge.z_);
            corners[3] = core::vector3df(box.MinEdge.x_, box.MaxEdge.y_, box.MaxEdge.z_);
            corners[4] = core::vector3df(box.MaxEdge.x_, box.MinEdge.y_, box.MinEdge.z_);
            corners[5] = core::vector3df(box.MaxEdge.x_, box.MinEdge.y_, box.MaxEdge.z_);
            corners[6] = core::vector3df(box.MaxEdge.x_, box.MaxEdge.y_, box.MinEdge.z_);
            corners[7] = core::vector3df(box.MaxEdge.x_, box.MaxEdge.y_, box.MaxEdge.z_);

            core::Matrixf view_transform = camera_->GetViewTransform();
            corners[0] = view_transform.Apply(corners[0]);
            corners[1] = view_transform.Apply(corners[1]);
            corners[2] = view_transform.Apply(corners[2]);
            corners[3] = view_transform.Apply(corners[3]);
            corners[4] = view_transform.Apply(corners[4]);
            corners[5] = view_transform.Apply(corners[5]);
            corners[6] = view_transform.Apply(corners[6]);
            corners[7] = view_transform.Apply(corners[7]);

            for (u32 i = 0; i < 8; ++i)
            {
                light_bounding_box_.addInternalPoint(corners[i]);
            }
        }

        void CLightSceneNode::DoLightRecalc()
        {
            if ((light_data_.type_ == video::ELT_SPOT) || (light_data_.type_ == video::ELT_DIRECTIONAL))
            {
                light_data_.direction_ = core::vector3df(.0f, .0f, 1.0f);
                GetAbsoluteTransformation().RotateVec(light_data_.direction_);
                light_data_.direction_.Normalize();
            }
            if ((light_data_.type_ == video::ELT_SPOT) || (light_data_.type_ == video::ELT_POINT))
            {
                //const f32 r = light_data_.Radius * light_data_.Radius * 0.5f;
                //BBox.MaxEdge.set(r, r, r);
                //BBox.MinEdge.set(-r, -r, -r);
                ////setAutomaticCulling( scene::EAC_BOX );
                //setAutomaticCulling(scene::EAC_OFF);
                light_data_.position_ = GetAbsolutePosition();
            }
            //if (light_data_.Type == video::ELT_DIRECTIONAL)
            //{
            //    BBox.reset(0, 0, 0);
            //    setAutomaticCulling(scene::EAC_OFF);
            //}
            DoCameraRecalc();
        }

        void CLightSceneNode::DoCameraRecalc()
        {
            if (camera_ == nullptr)
                return;

            camera_->LookAt(light_data_.position_ + light_data_.direction_);
            camera_->SetUp(light_data_.direction_ + core::vector3df(0.f, 1.f, 0.f));
            core::vector3df light_direction = light_data_.direction_;
            light_direction.Normalize();
            switch (light_data_.type_)
            {
            case video::ELT_DIRECTIONAL:
                camera_->SetEye(light_direction * -10.f);
                break;
            case video::ELT_SPOT:
            case video::ELT_POINT:
                camera_->SetEye(light_data_.position_);
            default:;
            }
        }

        void CLightSceneNode::ResetCamera(bool delete_camera)
        {
            if (delete_camera)
            {
                delete camera_;
            }
            if (light_data_.type_ == video::ELT_DIRECTIONAL)
            {
                camera_ = new COrthogonalCameraSceneNode(this, scene_manager_, -1, 10, 1, 0.1, 15);
            }
            else
            {
                camera_ = new CPerspectiveCameraSceneNode(this, scene_manager_, -1, light_data_.outer_cone_ * 2, 1.f, 0.1, 20);
            }

            camera_->LookAt(light_data_.position_ + light_data_.direction_);
            camera_->SetUp(light_data_.direction_ + core::vector3df(0.f, 1.f, 0.f));
            core::vector3df light_direction = light_data_.direction_;
            light_direction.Normalize();
            switch (light_data_.type_)
            {
            case video::ELT_DIRECTIONAL:
                camera_->SetEye(light_direction * -1.f);
                break;
            case video::ELT_SPOT:
            case video::ELT_POINT:
                camera_->SetEye(light_data_.position_);
            default: ;
            }
        }
    } // end namespace scene
} // end namespace kong