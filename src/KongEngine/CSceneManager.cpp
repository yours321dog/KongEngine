#include "CSceneManager.h"
#include "CCubeSceneNode.h"
#include "CPerspectiveCameraSceneNode.h"
#include "os.h"
#include "IFileSystem.h"
#include "IMeshLoader.h"
#include "IReadFile.h"
#include "CMeshSceneNode.h"
#include "CLightSceneNode.h"
#include "CPlaneSceneNode.h"
#include "COrthogonalCameraSceneNode.h"

#ifdef _KONG_COMPILE_WITH_OBJ_LOADER_
#include "CObjMeshFileLoader.h"
#endif

namespace kong
{
    namespace scene
    {
        CSceneManager::CSceneManager(video::IVideoDriver* driver, io::IFileSystem *fs)
            : ISceneNode(nullptr, nullptr), driver_(driver), shadow_color_(150, 0, 0, 0),
            ambient_light_(0, 0, 0, 0), active_camera_(nullptr), file_system_(fs), shadow_enable_(false), light_index_num_(0), main_light_index_(0)
        {
#ifdef _KONG_COMPILE_WITH_OBJ_LOADER_
            MeshLoaderList.PushBack(new COBJMeshFileLoader(this, fs));
#endif
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

        IAnimatedMesh* CSceneManager::getMesh(const io::path& filename)
        {
            IAnimatedMesh* msh = nullptr;

            io::IReadFile* file = file_system_->CreateAndOpenFile(filename);
            if (!file)
            {
                os::Printer::log("Could not load mesh, because file could not be opened: ", filename, ELL_ERROR);
                return 0;
            }

            // iterate the list in reverse order so user-added loaders can override the built-in ones
            s32 count = MeshLoaderList.Size();
            for (s32 i = count - 1; i >= 0; --i)
            {
                if (MeshLoaderList[i]->isALoadableFileExtension(filename))
                {
                    // reset file to avoid side effects of previous calls to createMesh
                    file->Seek(0);
                    msh = MeshLoaderList[i]->createMesh(file);
                    if (msh != nullptr)
                    {
                        //MeshCache->addMesh(filename, msh);
                        //msh->drop();
                        break;
                    }
                }
            }

            delete file;

            if (msh == nullptr)
                os::Printer::log("Could not load mesh, file format seems to be unsupported", filename, ELL_ERROR);
            else
                os::Printer::log("Loaded mesh", filename, ELL_INFORMATION);

            return msh;
        }

        IAnimatedMesh* CSceneManager::getMesh(io::IReadFile* file)
        {
            if (!file)
                return 0;

            io::path name = file->GetFileName();
            //IAnimatedMesh* msh = MeshCache->getMeshByName(file->getFileName());
            IAnimatedMesh* msh = nullptr;
            if (msh != nullptr)
                return msh;

            // iterate the list in reverse order so user-added loaders can override the built-in ones
            s32 count = MeshLoaderList.Size();
            for (s32 i = count - 1; i >= 0; --i)
            {
                if (MeshLoaderList[i]->isALoadableFileExtension(name))
                {
                    // reset file to avoid side effects of previous calls to createMesh
                    file->Seek(0);
                    msh = MeshLoaderList[i]->createMesh(file);
                    if (msh)
                    {
                        //MeshCache->addMesh(file->getFileName(), msh);
                        //msh->drop();
                        break;
                    }
                }
            }

            if (!msh)
                os::Printer::log("Could not load mesh, file format seems to be unsupported", file->GetFileName(), ELL_ERROR);
            else
                os::Printer::log("Loaded mesh", file->GetFileName(), ELL_INFORMATION);

            return msh;
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

        IMeshSceneNode* CSceneManager::AddPlaneSceneNode(f32 size, ISceneNode* parent, s32 id,
            const core::Vector3Df& position, const core::Vector3Df& rotation, const core::Vector3Df& scale)
        {
            if (parent == nullptr)
            {
                parent = this;
            }

            IMeshSceneNode *node = new CPlaneSceneNode(size, parent, this, id, position, rotation, scale);

            return node;
        }

        IMeshSceneNode* CSceneManager::AddMeshSceneNode(IMesh* mesh, ISceneNode* parent, s32 id,
            const core::vector3df& position, const core::vector3df& rotation, const core::vector3df& scale,
            bool alsoAddIfMeshPointerZero)
        {
            if (parent == nullptr)
            {
                parent = this;
            }

            if (!alsoAddIfMeshPointerZero && mesh == nullptr)
            {
                return nullptr;
            }

            IMeshSceneNode *node = new CMeshSceneNode(mesh, parent, this, id, position, rotation, scale);

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

            const core::Dimension2d<u32> window_size = driver_->GetScreenSize();
            const f32 aspect = static_cast<f32>(window_size.width_) / window_size.height_;
            ICameraSceneNode *node = new CPerspectiveCameraSceneNode(parent, this, id, position, up, lookat, 90, aspect);

            if (make_active)
            {
                SetActiveCamera(node);
            }

            return node;
        }

        ICameraSceneNode* CSceneManager::AddOrthogonalCameraSceneNode(ISceneNode* parent,
            const core::Vector3Df& position, const core::Vector3Df& up, const core::Vector3Df& lookat, s32 id,
            bool make_active)
        {
            if (parent == nullptr)
            {
                parent = this;
            }

            const core::Dimension2d<u32> window_size = driver_->GetScreenSize();
            const f32 aspect = static_cast<f32>(window_size.width_) / window_size.height_;
            ICameraSceneNode *node = new COrthogonalCameraSceneNode(parent, this, id, position, up, lookat, 1.f, aspect);

            if (make_active)
            {
                SetActiveCamera(node);
            }

            return node;
        }

        ILightSceneNode* CSceneManager::AddLightSceneNode(ISceneNode* parent, const core::vector3df& position,
            video::SColorf color, f32 radius, s32 id)
        {
            if (parent == nullptr)
            {
                parent = this;
            }

            ILightSceneNode* node = new CLightSceneNode(parent, this, id, position, color, radius, light_index_num_++);

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

        void CSceneManager::EnableShadow(bool flag)
        {
            shadow_enable_ = flag;
            driver_->EnableShadow(flag);
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

        void CSceneManager::DrawAllDeferred()
        {
            if (driver_ == nullptr)
            {
                return;
            }
#ifdef _DEBUG
            driver_->CheckError();
#endif
            driver_->SetMaterial(video::SMaterial());

            driver_->CheckError();

            driver_->SetTransform(video::ETS_PROJECTION, core::identity_matrix);
            driver_->SetTransform(video::ETS_VIEW, core::identity_matrix);
            driver_->SetTransform(video::ETS_WORLD, core::identity_matrix);
#ifdef _DEBUG
            driver_->CheckError();
#endif
            // do animations and other stuff.
            OnAnimate(0);


            // let all nodes register themselves
            OnRegisterSceneNode();

            // render first pass
            driver_->RenderFirstPass();


#ifdef _DEBUG
            driver_->CheckError();
#endif

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

            // render default objects
            {
                for (u32 i = 0; i < solid_node_list_.Size(); ++i)
                {
                    solid_node_list_[i].node_->Render();
                }

                //solid_node_list_.Resize(0);
            }

            // render second pass
            driver_->RenderSecondPass();

            //render lights scenes
            {
                // set camera again
                if (active_camera_ != nullptr)
                {
                    active_camera_->Render();
                    cam_world_pos_ = active_camera_->GetAbsolutePosition();
                }

                //    // Sort the lights by distance from the camera
                //core::vector3df cam_world_pos(0, 0, 0);
                //if (active_camera_ != nullptr)
                //    cam_world_pos = active_camera_->GetAbsolutePosition();

                driver_->DeleteAllDynamicLights();

                //Driver->setAmbientLight(AmbientLight);

                u32 max_lights = light_list_.Size();

                max_lights = core::min_(driver_->GetMaximalDynamicLightAmount(), max_lights);

                for (u32 i = 0; i< max_lights; ++i)
                    light_list_[i]->Render();
                driver_->ActivateDynamicLights();

                light_list_.Resize(0);
            }

            // render default objects
            {
                for (u32 i = 0; i < solid_node_list_.Size(); ++i)
                {
                    solid_node_list_[i].node_->Render();
                }

                solid_node_list_.Resize(0);
            }
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

            //render shadow
            if (shadow_enable_)
            {
                driver_->BeginShadowRender();

                // render lights
                dynamic_cast<ILightSceneNode *>(light_list_[0])->ResetCameraTransform(solid_node_list_);
                dynamic_cast<ILightSceneNode *>(light_list_[0])->RenderShadow();

                // render default objects
                {
                    for (u32 i = 0; i < solid_node_list_.Size(); ++i)
                    {
                        solid_node_list_[i].node_->Render();
                    }

                    //solid_node_list_.Resize(0);
                }

                driver_->EndShadowRender();
            }

            //render lights scenes
            {
                //    // Sort the lights by distance from the camera
                //core::vector3df cam_world_pos(0, 0, 0);
                //if (active_camera_ != nullptr)
                //    cam_world_pos = active_camera_->GetAbsolutePosition();

                driver_->DeleteAllDynamicLights();

                //Driver->setAmbientLight(AmbientLight);

                u32 max_lights = light_list_.Size();

                max_lights = core::min_(driver_->GetMaximalDynamicLightAmount(), max_lights);

                s32 idx_main_light = main_light_index_ < max_lights ? main_light_index_ : 0;
                for (u32 i = 0; i < max_lights; ++i)
                {
                    light_list_[i]->Render();
                }
                light_list_.Resize(0);
            }

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
                    light_list_.PushBack(node);
                    taken = 1;
                //}
                break;

            case ESNRP_SKY_BOX:
                //SkyBoxList.push_back(node);
                //taken = 1;
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

        IMeshManipulator* CSceneManager::GetMeshManipulator()
        {
            return driver_->GetMeshManipulator();
        }

        const core::aabbox3d<f32>& CSceneManager::GetBoundingBox() const
        {
            _KONG_DEBUG_BREAK_IF(true) // Bounding Box of Scene Manager wanted.

            // should never be used.
            return *(static_cast<core::aabbox3d<f32>*>(nullptr));
        }

        void CSceneManager::SetMainLight(const ILightSceneNode* light_node)
        {
            main_light_index_ = light_node->GetLightIndex();
        }

        ISceneManager* CreateSceneManager(video::IVideoDriver* driver,
            io::IFileSystem* fs/*, gui::ICursorControl* cc, gui::IGUIEnvironment *gui*/)
        {
            CSceneManager *sm = new CSceneManager(driver, fs);
            return sm;
        }
    } // end namespace scene
} // end namespace kong
