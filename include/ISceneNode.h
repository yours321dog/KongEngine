// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _ISCENENODE_H_
#define _ISCENENODE_H_
#include "Array.h"
#include "Matrix.h"
#include "List.h"
#include "SMaterial.h"
#include "aabbox3d.h"
#include "ESceneNodeType.h"
#include "ERenderingMode.h"
#include "SMesh.h"
#include "CMeshBuffer.h"
#include "SBoundingBoxMesh.h"

namespace kong
{
    namespace scene
    {
        class ISceneManager;

        //! Typedef for list of scene nodes
        //typedef core::List<ISceneNode*> ISceneNodeList;
        //! Typedef for list of scene node animators
        //typedef core::list<ISceneNodeAnimator*> ISceneNodeAnimatorList;

        class ISceneNode
        {
        public:
            virtual ~ISceneNode()
            {
                children_.clear();
            }

            ISceneNode(ISceneNode *parent, ISceneManager * mgr, s32 id = -1,
                const core::Vector3Df &position = core::Vector3Df(0.f, 0.f, 0.f),
                const core::Vector3Df &rotation = core::Vector3Df(0.f, 0.f, 0.f),
                const core::Vector3Df &scale = core::Vector3Df(1.f, 1.f, 1.f))
                : relative_translation_(position), relative_rotation_(rotation), relative_scale_(scale),
                parent_(nullptr), id_(id), scene_manager_(mgr), is_visible_(true), rendering_mode_(video::ERM_MESH), draw_bounding_box_(false)
            {
                if (parent != nullptr)
                {
                    parent->AddChild(this);
                }

                UpdateAbsolutePosition();
            }

            //! This method is called just before the rendering process of the whole scene.
            /** Nodes may register themselves in the render pipeline during this call,
            precalculate the geometry which should be renderered, and prevent their
            children from being able to register themselves if they are clipped by simply
            not calling their OnRegisterSceneNode method.
            If you are implementing your own scene node, you should overwrite this method
            with an implementation code looking like this:
            \code
            if (IsVisible)
            SceneManager->registerNodeForRendering(this);

            ISceneNode::OnRegisterSceneNode();
            \endcode
            */
            virtual void OnRegisterSceneNode()
            {
                if (is_visible_)
                {
                    core::List<ISceneNode*>::Iterator it = children_.begin();
                    for (; it != children_.end(); ++it)
                    {
                        (*it)->OnRegisterSceneNode();
                    }
                }
            }

            //! Adds a child to this scene node.
            /** If the scene node already has a parent it is first removed
            from the other parent.
            \param child A pointer to the new child. */
            virtual void AddChild(ISceneNode* child)
            {
                if (child && (child != this))
                {
                    // Change scene manager?
                    if (scene_manager_ != child->scene_manager_)
                        child->SetSceneManager(scene_manager_);

                    //child->grab();
                    child->Remove(); // remove from old parent
                    children_.push_back(child);
                    child->parent_ = this;
                }
            }

            //! Removes a child from this scene node.
            /** If found in the children list, the child pointer is also
            dropped and might be deleted if no other grab exists.
            \param child A pointer to the child which shall be removed.
            \return True if the child was removed, and false if not,
            e.g. because it couldn't be found in the children list. */
            virtual bool RemoveChild(ISceneNode* child)
            {
                core::List<ISceneNode*>::Iterator it = children_.begin();
                for (; it != children_.end(); ++it)
                    if ((*it) == child)
                    {
                        (*it)->parent_ = nullptr;
                        //(*it)->drop();
                        children_.erase(it);
                        return true;
                    }

                _KONG_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
                return false;
            }

            //! Removes all children of this scene node
            /** The scene nodes found in the children list are also dropped
            and might be deleted if no other grab exists on them.
            */
            virtual void RemoveAll()
            {
                core::List<ISceneNode*>::Iterator it = children_.begin();
                for (; it != children_.end(); ++it)
                {
                    (*it)->parent_ = nullptr;
                    //(*it)->drop();
                }

                children_.clear();
            }

            //! Removes this scene node from the scene
            /** If no other grab exists for this node, it will be deleted.
            */
            virtual void Remove()
            {
                if (parent_)
                    parent_->RemoveChild(this);
            }

            /** NOTE: For speed reasons the absolute transformation is not
            automatically recalculated on each change of the relative
            transformation or by a transformation change of an parent. Instead the
            update usually happens once per frame in OnAnimate. You can enforce
            an update with updateAbsolutePosition().
            \return The absolute transformation matrix. */
            virtual const core::Matrixf& GetAbsoluteTransformation() const
            {
                return absolute_tranform_;
            }

            //! Returns the relative transformation of the scene node.
            /** The relative transformation is stored internally as 3
            vectors: translation, rotation and scale. To get the relative
            transformation matrix, it is calculated from these values.
            \return The relative transformation matrix. */
            virtual core::Matrixf GetRelativeTransformation() const
            {
                core::Matrixf mat(core::Matrixf::IDENTITY);
                mat.Rotate(relative_rotation_.x_, relative_rotation_.y_, relative_rotation_.z_);
                mat.Translate(relative_translation_.x_, relative_translation_.y_, relative_translation_.z_);

                if (relative_scale_ != core::Vector3Df(1.f, 1.f, 1.f))
                {
                    core::Matrixf smat(core::Matrixf::IDENTITY);
                    smat.Scale(relative_scale_.x_, relative_scale_.y_, relative_scale_.z_);
                    mat = mat * smat;
                }

                return mat;
            }

            //! Updates the absolute position based on the relative and the parents position
            /** Note: This does not recursively update the parents absolute positions, so if you have a deeper
            hierarchy you might want to update the parents first.*/
            virtual void UpdateAbsolutePosition()
            {
                if (parent_)
                {
                    absolute_tranform_ =
                        GetRelativeTransformation() * parent_->GetAbsoluteTransformation();
                }
                else
                    absolute_tranform_ = GetRelativeTransformation();
            }

            //! Renders the node.
            virtual void Render() = 0;

            //! Returns whether the node should be visible (if all of its parents are visible).
            /** This is only an option set by the user, but has nothing to
            do with geometry culling
            \return The requested visibility of the node, true means
            visible (if all parents are also visible). */
            virtual bool IsVisible() const
            {
                _KONG_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
                return is_visible_;
            }

            //! Check whether the node is truly visible, taking into accounts its parents' visibility
            /** \return true if the node and all its parents are visible,
            false if this or any parent node is invisible. */
            virtual bool IsTrulyVisible() const
            {
                _KONG_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
                if (!is_visible_)
                    return false;

                if (!parent_)
                    return true;

                return parent_->IsTrulyVisible();
            }

            //! Sets if the node should be visible or not.
            /** All children of this node won't be visible either, when set
            to false. Invisible nodes are not valid candidates for selection by
            collision manager bounding box methods.
            \param is_visible If the node shall be visible. */
            virtual void SetVisible(bool is_visible)
            {
                is_visible_ = is_visible;
            }

            //! Get the id of the scene node.
            /** This id can be used to identify the node.
            \return The id. */
            virtual s32 GetID() const
            {
                return id_;
            }


            //! Sets the id of the scene node.
            /** This id can be used to identify the node.
            \param id The new id. */
            virtual void SetID(s32 id)
            {
                id_ = id;
            }

            //! Returns the material based on the zero based index i.
            /** To get the amount of materials used by this scene node, use
            getMaterialCount(). This function is needed for inserting the
            node into the scene hierarchy at an optimal position for
            minimizing renderstate changes, but can also be used to
            directly modify the material of a scene node.
            \param num Zero based index. The maximal value is getMaterialCount() - 1.
            \return The material at that index. */
            virtual video::SMaterial& GetMaterial(u32 num)
            {
                return video::IdentityMaterial;
            }

            //! Get amount of materials used by this scene node.
            /** \return Current amount of materials of this scene node. */
            virtual u32 GetMaterialCount() const
            {
                return 0;
            }

            //! Gets the scale of the scene node relative to its parent.
            /** This is the scale of this node relative to its parent.
            If you want the absolute scale, use
            getAbsoluteTransformation().getScale()
            \return The scale of the scene node. */
            virtual const core::Vector3Df& GetScale() const
            {
                return relative_scale_;
            }


            //! Sets the relative scale of the scene node.
            /** \param scale New scale of the node, relative to its parent. */
            virtual void SetScale(const core::Vector3Df& scale)
            {
                relative_scale_ = scale;
            }


            //! Gets the rotation of the node relative to its parent.
            /** Note that this is the relative rotation of the node.
            If you want the absolute rotation, use
            getAbsoluteTransformation().getRotation()
            \return Current relative rotation of the scene node. */
            virtual const core::Vector3Df& GetRotation() const
            {
                return relative_rotation_;
            }


            //! Sets the rotation of the node relative to its parent.
            /** This only modifies the relative rotation of the node.
            \param rotation New rotation of the node in degrees. */
            virtual void SetRotation(const core::Vector3Df& rotation)
            {
                relative_rotation_ = rotation;
            }


            //! Gets the position of the node relative to its parent.
            /** Note that the position is relative to the parent. If you want
            the position in world coordinates, use getAbsolutePosition() instead.
            \return The current position of the node relative to the parent. */
            virtual const core::Vector3Df& GetPosition() const
            {
                return relative_translation_;
            }


            //! Sets the position of the node relative to its parent.
            /** Note that the position is relative to the parent.
            \param newpos New relative position of the scene node. */
            virtual void SetPosition(const core::Vector3Df& newpos)
            {
                relative_translation_ = newpos;
            }

            //! Gets the absolute position of the node in world coordinates.
            /** If you want the position of the node relative to its parent,
            use getPosition() instead.
            NOTE: For speed reasons the absolute position is not
            automatically recalculated on each change of the relative
            position or by a position change of an parent. Instead the
            update usually happens once per frame in OnAnimate. You can enforce
            an update with updateAbsolutePosition().
            \return The current absolute position of the scene node (updated on last call of updateAbsolutePosition). */
            virtual core::Vector3Df GetAbsolutePosition() const
            {
                return absolute_tranform_.GetTranslation();
            }

            //! OnAnimate() is called just before rendering the whole scene.
            /** Nodes may calculate or store animations here, and may do other useful things,
            depending on what they are. Also, OnAnimate() should be called for all
            child scene nodes here. This method will be called once per frame, independent
            of whether the scene node is visible or not.
            \param timeMs Current time in milliseconds. */
            virtual void OnAnimate(u32 timeMs)
            {
                if (is_visible_)
                {
                    //// animate this node with all animators

                    //ISceneNodeAnimatorList::Iterator ait = Animators.begin();
                    //while (ait != Animators.end())
                    //{
                    //    // continue to the next node before calling animateNode()
                    //    // so that the animator may remove itself from the scene
                    //    // node without the iterator becoming invalid
                    //    ISceneNodeAnimator* anim = *ait;
                    //    ++ait;
                    //    anim->animateNode(this, timeMs);
                    //}

                    // update absolute position
                    UpdateAbsolutePosition();

                    // perform the post render process on all children

                    core::List<ISceneNode*>::Iterator it = children_.begin();
                    for (; it != children_.end(); ++it)
                    {
                        (*it)->OnAnimate(timeMs);
                    }
                }
            }

            //! Sets the texture of the specified layer in all materials of this scene node to the new texture.
            /** \param textureLayer Layer of texture to be set. Must be a
            value smaller than MATERIAL_MAX_TEXTURES.
            \param texture New texture to be used. */
            void SetMaterialTexture(u32 textureLayer, video::ITexture* texture)
            {
                if (textureLayer >= video::MATERIAL_MAX_TEXTURES)
                    return;

                for (u32 i = 0; i<GetMaterialCount(); ++i)
                    GetMaterial(i).SetTexture(textureLayer, texture);
            }

            //! Get the axis aligned, not transformed bounding box of this node.
            /** This means that if this node is an animated 3d character,
            moving in a room, the bounding box will always be around the
            origin. To get the box in real world coordinates, just
            transform it with the matrix you receive with
            getAbsoluteTransformation() or simply use
            getTransformedBoundingBox(), which does the same.
            \return The non-transformed bounding box. */
            virtual const core::aabbox3d<f32>& GetBoundingBox() const = 0;

            //! Get the axis aligned, transformed and animated absolute bounding box of this node.
            /** \return The transformed bounding box. */
            virtual core::aabbox3d<f32> GetTransformedBoundingBox() const
            {
                core::aabbox3d<f32> box = GetBoundingBox();
                absolute_tranform_.TransformBoxEx(box);
                return box;
            }

            //! Nomalize the vertices of buffers
            virtual void NormalizeVertice()
            {
            }

            //! Returns type of the scene node
            /** \return The type of this node. */
            virtual ESCENE_NODE_TYPE GetType() const
            {
                return ESNT_UNKNOWN;
            }

            //! Set rendering mode
            /** This method will change the rendering mode.
            * \param mode switch between wireframe and mesh rendering mode */
            virtual void SetRenderingMode(video::E_RENDERING_MODE mode = video::ERM_MESH)
            {
                rendering_mode_ = mode;
            }

            //! Draw bounding box
            /** This method will enable the bounding box drawing
             * \param draw_bounding_box Draw bounding box or not */
            virtual void EnableDrawBoundingBox(const bool draw_bounding_box = true)
            {
                draw_bounding_box_ = draw_bounding_box;
            }

            //! Rebuild the boundingbox
            virtual void RebuildBoundingBoxMesh() const
            {
                bounding_box_mesh_.RebuildBoundingBoxMesh(GetTransformedBoundingBox());
            }

        protected:
            //! Sets the new scene manager for this node and all children.
            //! Called by addChild when moving nodes between scene managers
            void SetSceneManager(ISceneManager* new_manager)
            {
                scene_manager_ = new_manager;

                core::List<ISceneNode*>::Iterator it = children_.begin();
                for (; it != children_.end(); ++it)
                    (*it)->SetSceneManager(new_manager);
            }

            c8 name_[100];
            core::Matrixf absolute_tranform_;
            core::Vector3Df relative_translation_;
            core::Vector3Df relative_rotation_;
            core::Vector3Df relative_scale_;

            ISceneNode *parent_;
            core::List<ISceneNode*> children_;
            s32 id_;

            //! Pointer to the scene manager
            ISceneManager* scene_manager_;

            bool is_visible_;

            //! rendering mode
            video::E_RENDERING_MODE rendering_mode_;

            static core::Array<core::Vector3Df> barycentric_;

            //! draw bounding box
            bool draw_bounding_box_;

            SBoundingBoxMesh bounding_box_mesh_;
        };
    } // end namespace scene
} // end namespace kong

#endif