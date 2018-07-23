// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _CLIGHTSCENENODE_H_
#define _CLIGHTSCENENODE_H_

#include "ILightSceneNode.h"

namespace kong
{
    namespace scene
    {
        class CLightSceneNode : public ILightSceneNode
        {
        public :
            //! constructor
            CLightSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
                const core::vector3df& position, video::SColorf color, f32 radius);

            virtual ~CLightSceneNode();

            //! pre render event
            void OnRegisterSceneNode() override;

            //! render
            void Render() override;

            //! returns the axis aligned bounding box of this node
            const core::aabbox3d<f32>& GetBoundingBox() const override;

            //! set node light data from light info
            void SetLightData(const video::SLight& light) override;

            //! \return Returns the light data.
            const video::SLight& GetLightData() const override;

            //! \return Returns the light data.
            video::SLight& GetLightData() override;

            //! Sets if the node should be visible or not.
            /** All children of this node won't be visible either, when set
            to true.
            \param isVisible If the node shall be visible. */
            void SetVisible(bool isVisible) override;

            //! Sets the light type.
            /** \param type The new type. */
            void SetLightType(video::E_LIGHT_TYPE type) override;

            //! Returns type of the scene node
            video::E_LIGHT_TYPE GetLightType() const override;

            //! Returns type of the scene node
            ESCENE_NODE_TYPE GetType() const override { return ESNT_LIGHT; }

            //! Sets the light's radius of influence.
            void SetRadius(f32 radius) override;

            //! Gets the light's radius of influence.
            /** \return The current radius. */
            f32 GetRadius() const override;

        private:
            void DoLightRecalc();

            video::SLight light_data_;
            core::aabbox3d<f32> box_;
            s32 driver_light_index_;
            bool light_is_on_;

        };
    }
}

#endif
