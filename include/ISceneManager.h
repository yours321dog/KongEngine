// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _ISCENEMANAGER_H_
#define _ISCENEMANAGER_H_
#include "ISceneNode.h"
#include "IVideoDriver.h"

namespace kong
{
    namespace video
    {
        class IVideoDriver;
        class SMaterial;
        //class IImage;
        //class ITexture;
    } // end namespace video

    namespace scene
    {
        class ICameraSceneNode;
        class IMeshSceneNode;
        class ISceneNode;

        class ISceneManager
        {
        public:
            virtual ~ISceneManager() = default;

            //! Adds a cube scene node
            /** \param size: Size of the cube, uniformly in each dimension.
            \param parent: Parent of the scene node. Can be 0 if no parent.
            \param id: Id of the node. This id can be used to identify the scene node.
            \param position: Position of the space relative to its parent
            where the scene node will be placed.
            \param rotation: Initital rotation of the scene node.
            \param scale: Initial scale of the scene node.
            \return Pointer to the created test scene node. This
            pointer should not be dropped. See IReferenceCounted::drop()
            for more information. */
            virtual IMeshSceneNode* AddCubeSceneNode(f32 size = 10.0f, ISceneNode* parent = 0, s32 id = -1,
                const core::Vector3Df& position = core::Vector3Df(0, 0, 0),
                const core::Vector3Df& rotation = core::Vector3Df(0, 0, 0),
                const core::Vector3Df& scale = core::Vector3Df(1.0f, 1.0f, 1.0f)) = 0;

            //! Adds a camera scene node to the scene graph and sets it as active camera.
            /** This camera does not react on user input like for example the one created with
            addCameraSceneNodeFPS(). If you want to move or animate it, use animators or the
            ISceneNode::setPosition(), ICameraSceneNode::setTarget() etc methods.
            By default, a camera's look at position (set with setTarget()) and its scene node
            rotation (set with setRotation()) are independent. If you want to be able to
            control the direction that the camera looks by using setRotation() then call
            ICameraSceneNode::bindTargetAndRotation(true) on it.
            \param position: Position of the space relative to its parent where the camera will be placed.
            \param up
            \param lookat: Position where the camera will look at. Also known as target.
            \param parent: Parent scene node of the camera. Can be null. If the parent moves,
            the camera will move too.
            \param id: id of the camera. This id can be used to identify the camera.
            \param makeActive Flag whether this camera should become the active one.
            Make sure you always have one active camera.
            \return Pointer to interface to camera if successful, otherwise 0.
            This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
            virtual ICameraSceneNode* AddCameraSceneNode(ISceneNode* parent = nullptr,
                const core::Vector3Df& position = core::Vector3Df(0, 0, 0),
                const core::Vector3Df& up = core::Vector3Df(0, 1, 0),
                const core::Vector3Df& lookat = core::Vector3Df(0, 0, 100),
                s32 id = -1, bool make_active = true) = 0;

            //! Adds a camera scene node to the scene graph and sets it as active camera.
            /** This camera does not react on user input like for example the one created with
            addCameraSceneNodeFPS(). If you want to move or animate it, use animators or the
            ISceneNode::setPosition(), ICameraSceneNode::setTarget() etc methods.
            By default, a camera's look at position (set with setTarget()) and its scene node
            rotation (set with setRotation()) are independent. If you want to be able to
            control the direction that the camera looks by using setRotation() then call
            ICameraSceneNode::bindTargetAndRotation(true) on it.
            \param position: Position of the space relative to its parent where the camera will be placed.
            \param up
            \param lookat: Position where the camera will look at. Also known as target.
            \param parent: Parent scene node of the camera. Can be null. If the parent moves,
            the camera will move too.
            \param id: id of the camera. This id can be used to identify the camera.
            \param makeActive Flag whether this camera should become the active one.
            Make sure you always have one active camera.
            \return Pointer to interface to camera if successful, otherwise 0.
            This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
            virtual ICameraSceneNode* AddPerspectiveCameraSceneNode(ISceneNode* parent = nullptr,
                const core::Vector3Df& position = core::Vector3Df(0, 0, 0),
                const core::Vector3Df& up = core::Vector3Df(0, 1, 0),
                const core::Vector3Df& lookat = core::Vector3Df(0, 0, 100),
                s32 id = -1, bool make_active = true) = 0;

            //! Adds a scene node to the scene by name
            /** \return Pointer to the scene node added by a factory
            This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
            virtual ISceneNode* AddSceneNode(const char* scene_node_name, ISceneNode* parent = nullptr) = 0;

            //! Sets the currently active camera.
            /** The previous active camera will be deactivated.
            \param camera: The new camera which should be active. */
            virtual void SetActiveCamera(ICameraSceneNode* camera) = 0;

            //! Get the current color of shadows.
            virtual video::SColor GetShadowColor() const = 0;

            //! Set the current color of shadows.
            virtual void SetShadowColor(video::SColor color) = 0;

            //! Draws all the scene nodes.
            /** This can only be invoked between
            IVideoDriver::beginScene() and IVideoDriver::endScene(). Please note that
            the scene is not only drawn when calling this, but also animated
            by existing scene node animators, culling of scene nodes is done, etc. */
            virtual void DrawAll() = 0;

            //! Clears the whole scene.
            /** All scene nodes are removed. */
            virtual void Clear() = 0;

            //! Get video driver
            virtual video::IVideoDriver *GetVideoDriver() const = 0;
        };
    }
}

#endif