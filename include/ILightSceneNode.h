// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _ILIGHTSCENENODE_H_
#define _ILIGHTSCENENODE_H_

#include "ISceneNode.h"
#include "SLight.h"

namespace kong
{
    namespace scene
    {
        class ILightSceneNode : public ISceneNode
        {
        public:

            //! constructor
            ILightSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
                const core::vector3df& position = core::vector3df(0, 0, 0))
                : ISceneNode(parent, mgr, id, position) {}

            virtual ~ILightSceneNode() = default;

            //! Sets the light data associated with this ILightSceneNode
            /** \param light The new light data. */
            virtual void SetLightData(const video::SLight& light) = 0;

            //! Gets the light data associated with this ILightSceneNode
            /** \return The light data. */
            virtual const video::SLight& GetLightData() const = 0;

            //! Gets the light data associated with this ILightSceneNode
            /** \return The light data. */
            virtual video::SLight& GetLightData() = 0;

            //! Sets if the node should be visible or not.
            /** All children of this node won't be visible either, when set
            to true.
            \param isVisible If the node shall be visible. */
            virtual void SetVisible(bool isVisible) = 0;

            //! Sets the light type.
            /** \param type The new type. */
            virtual void SetLightType(video::E_LIGHT_TYPE type) = 0;

            //! Gets the light type.
            /** \return The current light type. */
            virtual video::E_LIGHT_TYPE GetLightType() const = 0;
        };
    }
}
#endif