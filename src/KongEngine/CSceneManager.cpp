#include "CSceneManager.h"
#include "CCubeSceneNode.h"
#include "CPerspectiveCameraSceneNode.h"

namespace kong
{
    namespace scene
    {
        CSceneManager::CSceneManager(video::IVideoDriver* driver)
            : ISceneNode(nullptr, nullptr), driver_(driver), shadow_color_(150, 0, 0, 0),
            ambient_light_(0, 0, 0, 0), active_camera_(nullptr)
        {
            // root node's scene manager
            scene_manager_ = this;
        }

        CSceneManager::~CSceneManager()
        {
            for (u32 i = 0; i < camera_list_.Size(); i++)
            {
                delete camera_list_[i];
            }
            camera_list_.Clear();

            for (u32 i = 0; i < solid_node_list_.Size(); i++)
            {
                delete solid_node_list_[i].node_;
            }
            solid_node_list_.Clear();

            for (u32 i = 0; i < light_list_.Size(); i++)
            {
                delete light_list_[i];
            }
            light_list_.Clear();

            for (u32 i = 0; i < shadow_node_list_.Size(); i++)
            {
                delete shadow_node_list_[i];
            }
            shadow_node_list_.Clear();
        }

        IMeshSceneNode* CSceneManager::AddCubeSceneNode(f32 size, ISceneNode* parent, s32 id,
            const core::Vector3Df& position, const core::Vector3Df& rotation, const core::Vector3Df& scale)
        {
            if (parent == nullptr)
            {
                parent = this;
            }

            IMeshSceneNode *node = new CCubeSceneNode(size, parent, this, id, position, rotation, scale);

            return node;
        }

        ICameraSceneNode* CSceneManager::AddCameraSceneNode(ISceneNode* parent, const core::Vector3Df& position,
            const core::Vector3Df& up, const core::Vector3Df& lookat, s32 id, bool make_active)
        {
            return nullptr;
        }

        ICameraSceneNode* CSceneManager::AddPerspectiveCameraSceneNode(ISceneNode* parent,
            const core::Vector3Df& position, const core::Vector3Df& up, const core::Vector3Df& lookat, s32 id, bool make_active)
        {
            if (parent == nullptr)
            {
                parent = this;
            }

            ICameraSceneNode *node = new CPerspectiveCameraSceneNode(parent, this, id, position, up, lookat, 90);

            if (make_active)
            {
                SetActiveCamera(node);
            }

            return node;
        }

        ISceneNode* CSceneManager::AddSceneNode(const char* scene_node_name, ISceneNode* parent)
        {
            return nullptr;
        }

        void CSceneManager::SetShadowColor(video::SColor color)
        {
            shadow_color_ = color;
        }

        video::SColor CSceneManager::GetShadowColor() const
        {
            return shadow_color_;
        }

        void CSceneManager::SetActiveCamera(ICameraSceneNode* camera)
        {
            active_camera_ = camera;
        }

        ICameraSceneNode* CSceneManager::GetActiveCamera()
        {
            return active_camera_;
        }

        void CSceneManager::Clear()
        {
            RemoveAll();
        }

        void CSceneManager::Render()
        {
        }

        void CSceneManager::RemoveAll()
        {
            ISceneNode::RemoveAll();
            SetActiveCamera(nullptr);
            if (driver_ != nullptr)
            {
                driver_->SetMaterial(video::SMaterial());
            }
        }

        void CSceneManager::DrawAll()
        {
            if (driver_ == nullptr)
            {
                return;
            }

            driver_->SetMaterial(video::SMaterial());
            driver_->SetTransform(video::ETS_PROJECTION, core::identity_matrix);
            driver_->SetTransform(video::ETS_VIEW, core::identity_matrix);
            driver_->SetTransform(video::ETS_WORLD, core::identity_matrix);

            // do animations and other stuff.
            OnAnimate(0);

            /*!
            First Scene Node for prerendering should be the active camera
            consistent Camera is needed for culling
            */
            cam_world_pos_.Set(0, 0, 0);
            if (active_camera_ != nullptr)
            {
                active_camera_->Render();
                cam_world_pos_ = active_camera_->GetAbsolutePosition();
            }

            // let all nodes register themselves
            OnRegisterSceneNode();

            // render default objects
            {
                for (u32 i = 0; i < solid_node_list_.Size(); ++i)
                {
                    solid_node_list_[i].node_->Render();
                }

                solid_node_list_.Resize(0);
            }
        }

        video::IVideoDriver* CSceneManager::GetVideoDriver() const
        {
            return driver_;
        }

        u32 CSceneManager::RegisterNodeForRendering(ISceneNode* node, E_SCENE_NODE_RENDER_PASS pass)
        {
            u32 taken = 0;

            switch (pass)
            {
                // take camera if it is not already registered
            case ESNRP_CAMERA:
                {
                    taken = 1;
                    for (u32 i = 0; i != camera_list_.Size(); ++i)
                    {
                        if (camera_list_[i] == node)
                        {
                            taken = 0;
                            break;
                        }
                    }
                    if (taken)
                    {
                        camera_list_.PushBack(node);
                    }
                }
                break;

            case ESNRP_LIGHT:
                // TODO: Point Light culling..
                // Lighting model in irrlicht has to be redone..
                //if (!isCulled(node))
                //{
                //    LightList.push_back(node);
                //    taken = 1;
                //}
                break;

            case ESNRP_SKY_BOX:
                //SkyBoxList.push_back(node);
                taken = 1;
                break;
            case ESNRP_SOLID:
                //if (!isCulled(node))
                {
                    solid_node_list_.PushBack(node);
                    taken = 1;
                }
                break;
            case ESNRP_TRANSPARENT:
                //if (!isCulled(node))
                //{
                //    TransparentNodeList.push_back(TransparentNodeEntry(node, camWorldPos));
                //    taken = 1;
                //}
                break;
            case ESNRP_TRANSPARENT_EFFECT:
                //if (!isCulled(node))
                //{
                //    TransparentEffectNodeList.push_back(TransparentNodeEntry(node, camWorldPos));
                //    taken = 1;
                //}
                break;
            case ESNRP_AUTOMATIC:
                //if (!isCulled(node))
                //{
                //    const u32 count = node->getMaterialCount();

                //    taken = 0;
                //    for (u32 i = 0; i<count; ++i)
                //    {
                //        video::IMaterialRenderer* rnd =
                //            Driver->getMaterialRenderer(node->getMaterial(i).MaterialType);
                //        if (rnd && rnd->isTransparent())
                //        {
                //            // register as transparent node
                //            TransparentNodeEntry e(node, camWorldPos);
                //            TransparentNodeList.push_back(e);
                //            taken = 1;
                //            break;
                //        }
                //    }

                //    // not transparent, register as solid
                //    if (!taken)
                //    {
                //        SolidNodeList.push_back(node);
                //        taken = 1;
                //    }
                //}
                break;
            case ESNRP_SHADOW:
                //if (!isCulled(node))
                //{
                //    ShadowNodeList.push_back(node);
                //    taken = 1;
                //}
                break;

            case ESNRP_NONE: // ignore this one
                break;
            }

#ifdef _KONG_SCENEMANAGER_DEBUG
            s32 index = Parameters.findAttribute("calls");
            Parameters.setAttribute(index, Parameters.getAttributeAsInt(index) + 1);

            if (!taken)
            {
                index = Parameters.findAttribute("culled");
                Parameters.setAttribute(index, Parameters.getAttributeAsInt(index) + 1);
            }
#endif

            return taken;
        }

        ISceneManager* CreateSceneManager(video::IVideoDriver* driver
            /*io::IFileSystem* fs, gui::ICursorControl* cc, gui::IGUIEnvironment *gui*/)
        {
            CSceneManager *sm = new CSceneManager(driver);
            return sm;
        }
    } // end namespace scene
} // end namespace kong
