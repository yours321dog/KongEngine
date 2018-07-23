// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "ISceneManager.h"
#include "CLightSceneNode.h"

namespace kong
{
    namespace scene
    {
        CLightSceneNode::CLightSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
            const core::vector3df& position, video::SColorf color, f32 radius)
            : ILightSceneNode(parent, mgr, id, position), driver_light_index_(-1), light_is_on_(true)
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
        }

        CLightSceneNode::~CLightSceneNode()
        = default;

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
        }

        const core::aabbox3d<f32>& CLightSceneNode::GetBoundingBox() const
        {
            return box_;
        }

        void CLightSceneNode::SetLightData(const video::SLight& light)
        {
            light_data_ = light;
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
        }
    } // end namespace scene
} // end namespace kong