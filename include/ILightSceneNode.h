// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _ILIGHTSCENENODE_H_
#define _ILIGHTSCENENODE_H_

#include "ISceneNode.h"
#include "SLight.h"
#include "DefaultNodeEntry.h"

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

            //! Sets the light's radius of influence.
            /** Outside this radius the light won't lighten geometry and cast no
            shadows. Setting the radius will also influence the attenuation, setting
            it to (0,1/radius,0). If you want to override this behavior, set the
            attenuation after the radius.
            \param radius The new radius. */
            virtual void SetRadius(f32 radius) = 0;

            //! Gets the light's radius of influence.
            /** \return The current radius. */
            virtual f32 GetRadius() const = 0;

            //! Get lihgt's transform matrix
            /** \return The current light transform. */
            virtual core::Matrixf GetLightTransform() const = 0;

            //! Render shadows
            virtual void RenderShadow() = 0;

            //! Reset camera transform
            virtual void ResetCameraTransform(core::Array<DefaultNodeEntry>& solid_nodes) = 0;

            //! Get main light index
            virtual s32 GetLightIndex() const = 0;
        };
    }
}
#endif