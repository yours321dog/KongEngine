// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _ISCENENODE_H_
#define _ISCENENODE_H_
#include "Array.h"
#include "Matrix.h"
#include "List.h"
#include "SMaterial.h"

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
            virtual ~ISceneNode() = default;

            ISceneNode(ISceneNode *parent, ISceneManager * mgr, s32 id = -1,
                const core::Vector3Df &position = core::Vector3Df(0.f, 0.f, 0.f),
                const core::Vector3Df &rotation = core::Vector3Df(0.f, 0.f, 0.f),
                const core::Vector3Df &scale = core::Vector3Df(1.f, 1.f, 1.f))
                : relative_translation_(position), relative_totation_(rotation), relative_scale_(scale),
                parent_(nullptr), id_(id), scene_manager_(mgr), is_visible_(true)
            {
                if (parent != nullptr)
                {
                    parent->AddChild(this);
                }

                UpdateAbsolutePosition();
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
                        child->setSceneManager(scene_manager_);

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
                core::Matrixf mat;
                mat.Rotate(relative_translation_.x, relative_totation_.y, relative_totation_.z);
                mat.Translate(relative_translation_.x, relative_translation_.y, relative_translation_.z);

                if (relative_scale_ != core::Vector3Df(1.f, 1.f, 1.f))
                {
                    core::Matrixf smat;
                    smat.Scale(relative_scale_.x, relative_scale_.y, relative_scale_.z);
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
                        parent_->GetRelativeTransformation() * GetAbsoluteTransformation();
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

        protected:
            //! Sets the new scene manager for this node and all children.
            //! Called by addChild when moving nodes between scene managers
            void setSceneManager(ISceneManager* newManager)
            {
                scene_manager_ = newManager;

                core::List<ISceneNode*>::Iterator it = children_.begin();
                for (; it != children_.end(); ++it)
                    (*it)->setSceneManager(newManager);
            }

            c8 name_[100]{};
            core::Matrixf absolute_tranform_;
            core::Vector3Df relative_translation_;
            core::Vector3Df relative_totation_;
            core::Vector3Df relative_scale_;

            ISceneNode *parent_;
            core::List<ISceneNode*> children_;
            s32 id_;

            //! Pointer to the scene manager
            ISceneManager* scene_manager_;

            bool is_visible_;

        };
    } // end namespace scene
} // end namespace kong

#endif