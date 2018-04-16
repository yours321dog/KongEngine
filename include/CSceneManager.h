// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _CSCENEMANAGER_H_
#define _CSCENEMANAGER_H_
#include "ISceneManager.h"
#include "Array.h"
#include "ISceneNode.h"
#include "ICameraSceneNode.h"
#include "IVideoDriver.h"

namespace kong
{
    namespace io {
        class IFileSystem;
    }
    namespace scene
    {
        class CSceneManager : public ISceneManager, public ISceneNode
        {
        public:
            //! constructor
            CSceneManager(video::IVideoDriver* driver, io::IFileSystem* fs/*,
                gui::ICursorControl* cursorControl, IMeshCache* cache = 0,
                gui::IGUIEnvironment *guiEnvironment = 0*/);

            //! destructor
            virtual ~CSceneManager();

            //! gets an animateable mesh. loads it if needed. returned pointer must not be dropped.
            virtual IAnimatedMesh* getMesh(const io::path& filename);

            //! gets an animateable mesh. loads it if needed. returned pointer must not be dropped.
            virtual IAnimatedMesh* getMesh(io::IReadFile* file);

            //! add cube scene node
            virtual IMeshSceneNode* AddCubeSceneNode(f32 size = 10.0f, ISceneNode* parent = nullptr, s32 id = -1,
                const core::Vector3Df& position = core::Vector3Df(0, 0, 0),
                const core::Vector3Df& rotation = core::Vector3Df(0, 0, 0),
                const core::Vector3Df& scale = core::Vector3Df(1.0f, 1.0f, 1.0f));

            //! Adds a scene node for rendering a static mesh.
            virtual IMeshSceneNode* AddMeshSceneNode(IMesh* mesh, ISceneNode* parent = 0, s32 id = -1,
                const core::vector3df& position = core::vector3df(0, 0, 0),
                const core::vector3df& rotation = core::vector3df(0, 0, 0),
                const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f),
                bool alsoAddIfMeshPointerZero = false);

            //! add camera scene node
            virtual ICameraSceneNode* AddCameraSceneNode(ISceneNode* parent = nullptr,
                const core::Vector3Df& position = core::Vector3Df(0, 0, 0),
                const core::Vector3Df& up = core::Vector3Df(0, 1, 0),
                const core::Vector3Df& lookat = core::Vector3Df(0, 0, 100),
                s32 id = -1, bool make_active = true);

            //! Adds a perspective camera scene node to the scene graph and sets it as active camera.
            virtual ICameraSceneNode* AddPerspectiveCameraSceneNode(ISceneNode* parent = nullptr,
                const core::Vector3Df& position = core::Vector3Df(0, 0, 0),
                const core::Vector3Df& up = core::Vector3Df(0, 1, 0),
                const core::Vector3Df& lookat = core::Vector3Df(0, 0, 100),
                s32 id = -1, bool make_active = true);

            //! add a normal scene node
            virtual ISceneNode* AddSceneNode(const char* scene_node_name, ISceneNode* parent = nullptr);

            //! Sets the color of stencil buffers shadows drawn by the scene manager.
            virtual void SetShadowColor(video::SColor color);

            //! Returns the current color of shadows.
            virtual video::SColor GetShadowColor() const;

            //! Sets the currently active camera.
            virtual void SetActiveCamera(ICameraSceneNode* camera);

            //! Get the currently active camera.
            virtual ICameraSceneNode *GetActiveCamera();

            //! Clears the whole scene. All scene nodes are removed.
            virtual void Clear();

            //! renders the node.
            virtual void Render();

            //! Removes all children of this scene node
            virtual void RemoveAll();

            //! Draws all the scene nodes.
            virtual void DrawAll();

            virtual video::IVideoDriver *GetVideoDriver() const;

            //! Registers a node for rendering it at a specific time.
            virtual u32 RegisterNodeForRendering(ISceneNode* node, E_SCENE_NODE_RENDER_PASS pass = ESNRP_AUTOMATIC);

            //! Get pointer to the mesh manipulator.
            virtual IMeshManipulator* GetMeshManipulator();

            //! returns the axis aligned bounding box of this node
            virtual const core::aabbox3d<f32>& GetBoundingBox() const;

        private:
            struct DefaultNodeEntry
            {
                DefaultNodeEntry(ISceneNode* n = nullptr) :
                    node_(n), texture_value_(nullptr)
                {
                    //if (n->GetMaterialCount())
                    //    TextureValue = (n->GetMaterial(0).GetTexture(0));
                }

                ~DefaultNodeEntry()
                {
                    //delete node_;
                }

                bool operator < (const DefaultNodeEntry& other) const
                {
                    return (texture_value_ < other.texture_value_);
                }

                ISceneNode* node_;
            private:
                void* texture_value_;
            };

            //! video driver
            video::IVideoDriver* driver_;

            core::Array<ISceneNode *> camera_list_;
            core::Array<ISceneNode *> light_list_;
            core::Array<ISceneNode *> shadow_node_list_;
            core::Array<DefaultNodeEntry> solid_node_list_;

            core::Array<IMeshLoader*> MeshLoaderList;

            video::SColor shadow_color_;
            video::SColor ambient_light_;

            ICameraSceneNode *active_camera_;
            core::Vector3Df cam_world_pos_;

            io::IFileSystem *file_system_;
        };
    }
}

#endif