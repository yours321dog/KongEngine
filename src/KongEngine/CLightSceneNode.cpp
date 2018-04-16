// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "CLightSceneNode.h"
#include "ISceneManager.h"

namespace kong
{
    namespace scene
    {
        CLightSceneNode::CLightSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
            const core::vector3df& position, video::SColorf color, f32 range)
            : ILightSceneNode(parent, mgr, id, position), driver_light_index_(-1), light_is_on_(false)
        {
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
    } // end namespace scene
} // end namespace kong