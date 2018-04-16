#include "CMeshSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "IMeshBuffer.h"

namespace kong
{
    namespace scene
    {
        CMeshSceneNode::CMeshSceneNode(IMesh* mesh, ISceneNode* parent, ISceneManager* mgr, s32 id,
            const core::vector3df& position, const core::vector3df& rotation, const core::vector3df& scale):
            IMeshSceneNode(parent, mgr, id, position, rotation, scale), mesh_(mesh)
        {
        }

        CMeshSceneNode::~CMeshSceneNode()
        {
            delete mesh_;
        }

        void CMeshSceneNode::SetMesh(IMesh* mesh)
        {
            if (mesh != nullptr)
            {
                mesh_ = mesh;
                CopyMaterials();
            }
        }

        IMesh* CMeshSceneNode::GetMesh()
        {
            return mesh_;
        }

        void CMeshSceneNode::OnRegisterSceneNode()
        {
            if (is_visible_)
            {
                // because this node supports rendering of mixed mode meshes consisting of
                // transparent and solid material at the same time, we need to go through all
                // materials, check of what type they are and register this node for the right
                // render pass according to that.

                PassCount = 0;                
                scene_manager_->RegisterNodeForRendering(this, scene::ESNRP_SOLID);

                ISceneNode::OnRegisterSceneNode();
            }
        }

        void CMeshSceneNode::Render()
        {
            video::IVideoDriver *driver = scene_manager_->GetVideoDriver();

            //for (u32 i = 0; i < 4; i++)
            //{
            //    for (u32 j = 0; j < 4; j++)
            //    {
            //        printf("%f\t", absolute_tranform_(i, j));
            //    }
            //    printf("\n");
            //}
            driver->SetTransform(video::ETS_WORLD, absolute_tranform_);

            for (u32 i = 0; i < mesh_->GetMeshBufferCount(); i++)
            {
                driver->SetMaterial(mesh_->GetMeshBuffer(i)->GetMaterial());
                driver->DrawMeshBuffer(mesh_->GetMeshBuffer(i));
            }
        }

        const core::aabbox3d<f32>& CMeshSceneNode::GetBoundingBox() const
        {
            return mesh_ != nullptr ? mesh_->GetBoundingBox() : box_;
        }

        void CMeshSceneNode::NormalizeVertice()
        {
            if (mesh_ == nullptr)
            {
                return;
            }

            core::Vector3Df min_point(1e10, 1e10, 1e10), max_point(-1e10, -1e10, -1e10);
            for (u32 i = 0; i < mesh_->GetMeshBufferCount(); i++)
            {
                IMeshBuffer *mesh_buffer = mesh_->GetMeshBuffer(i);
                for (u32 j = 0; j < mesh_buffer->GetVertexCount(); ++j)
                {
                    const core::Vector3Df pos = mesh_buffer->GetPosition(j);
                    min_point.x_ = core::min_(min_point.x_, pos.x_);
                    min_point.y_ = core::min_(min_point.y_, pos.y_);
                    min_point.z_ = core::min_(min_point.z_, pos.z_);

                    max_point.x_ = core::max_(max_point.x_, pos.x_);
                    max_point.y_ = core::max_(max_point.y_, pos.y_);
                    max_point.z_ = core::max_(max_point.z_, pos.z_);
                }
            }

            core::Vector3Df mid_point = (min_point + max_point) * 0.5f;
            const f32 len_x_inv = 1.f / (max_point.x_ - min_point.x_);
            const f32 len_y_inv = 1.f / (max_point.y_ - min_point.y_);
            const f32 len_z_inv = 1.f / (max_point.z_ - min_point.z_);

            for (u32 i = 0; i < mesh_->GetMeshBufferCount(); i++)
            {
                IMeshBuffer *mesh_buffer = mesh_->GetMeshBuffer(i);
                for (u32 j = 0; j < mesh_buffer->GetVertexCount(); ++j)
                {
                    core::Vector3Df &pos = mesh_buffer->GetPosition(j);
                    pos -= mid_point;
                    pos.x_ *= len_x_inv;
                    pos.y_ *= len_y_inv;
                    pos.z_ *= len_z_inv;
                }
            }

            for (u32 i = 0; i < mesh_->GetMeshBufferCount(); i++)
            {
                IMeshBuffer *mesh_buffer = mesh_->GetMeshBuffer(i);
                mesh_buffer->RecalculateBoundingBox();
            }

            mesh_->RecalculateBoundingBox();
        }

        void CMeshSceneNode::CopyMaterials()
        {
            materials_.Clear();

            if (mesh_ != nullptr)
            {
                video::SMaterial mat;

                for (u32 i = 0; i<mesh_->GetMeshBufferCount(); ++i)
                {
                    IMeshBuffer* mb = mesh_->GetMeshBuffer(i);
                    if (mb != nullptr)
                    {
                        mat = mb->GetMaterial();
                    }

                    materials_.PushBack(mat);
                }
            }
        }
    } // end namespace scene
}  // end namespace kong