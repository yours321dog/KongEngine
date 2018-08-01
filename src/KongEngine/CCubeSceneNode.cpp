// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "CCubeSceneNode.h"
#include "SMesh.h"
#include "CMeshBuffer.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "stdio.h"

namespace kong
{
    namespace scene
    {
        CCubeSceneNode::CCubeSceneNode(f32 size, ISceneNode* parent, ISceneManager* mgr, s32 id,
            const core::Vector3Df& position, const core::Vector3Df& rotation, const core::Vector3Df& scale)
            : IMeshSceneNode(parent, mgr, id, position, rotation, scale), size_(size)
        {
            GenerateMesh();
        }

        CCubeSceneNode::~CCubeSceneNode()
        {
            delete mesh_;
        }

        const core::aabbox3d<f32>& CCubeSceneNode::GetBoundingBox() const
        {
            return mesh_->GetMeshBuffer(0)->GetBoundingBox();
        }

        void CCubeSceneNode::Render()
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

            driver->SetRenderingMode(rendering_mode_);
            driver->SetTransform(video::ETS_WORLD, absolute_tranform_);

            driver->SetMaterial(mesh_->GetMeshBuffer(0)->GetMaterial());
            driver->DrawMeshBuffer(mesh_->GetMeshBuffer(0));

            if (draw_bounding_box_)
            {
                for (u32 i = 0; i < mesh_->GetMeshBufferCount(); i++)
                {
                    IMeshBuffer *mesh_buffer = mesh_->GetMeshBuffer(i);
                    mesh_buffer->RecalculateBoundingBox();
                }

                mesh_->RecalculateBoundingBox();
                RebuildBoundingBoxMesh();

                driver->SetRenderingMode(video::ERM_WIREFRAME);
                driver->SetTransform(video::ETS_WORLD, core::identity_matrix);
                //driver->SetTransform(video::ETS_WORLD, absolute_tranform_);
                driver->SetMaterial(bounding_box_mesh_.GetBoundingBoxMesh()->GetMeshBuffer(0)->GetMaterial());
                driver->DrawMeshBuffer(bounding_box_mesh_.GetBoundingBoxMesh()->GetMeshBuffer(0));
            }
        }

        video::SMaterial& CCubeSceneNode::GetMaterial(u32 num)
        {
            return mesh_->GetMeshBuffer(num)->GetMaterial();
        }

        u32 CCubeSceneNode::GetMaterialCount() const
        {
            return mesh_->GetMeshBufferCount();
        }

        void CCubeSceneNode::SetMesh(IMesh* mesh)
        {
            //delete mesh_;
            mesh_ = mesh;
        }

        IMesh* CCubeSceneNode::GetMesh()
        {
            return mesh_;
        }

        void CCubeSceneNode::OnRegisterSceneNode()
        {
            if (scene_manager_ != nullptr && IsVisible())
            {
                scene_manager_->RegisterNodeForRendering(this, ESNRP_SOLID);
            }
            ISceneNode::OnRegisterSceneNode();
        }

        void CCubeSceneNode::GenerateMesh()
        {
            SMeshBuffer *buffer = new SMeshBuffer();

            const u16 indices[36] = {
                0, 1, 2, 0, 2, 3,
                4, 5, 6, 4, 6, 7,
                8, 9, 10, 8, 10, 11,
                12, 13, 14, 12, 14, 15,
                16, 17, 18, 16, 18, 19,
                20, 21, 22, 20, 22, 23
            };

            const video::SColor clr_r = video::SColor(255, 255, 0, 0);
            const video::SColor clr_g = video::SColor(255, 0, 255, 0);
            const video::SColor clr_b = video::SColor(255, 0, 0, 255);

            buffer->vertices_.Reallocate(24);

            const f32 len = size_ / 2.f;

            buffer->vertices_.PushBack(video::S3DVertex(-len, -len, - len, 0.f, 0.f, -1.f, clr_r, 0.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, -len, -len, 0.f, 0.f, -1.f, clr_r, 1.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, len, -len, 0.f, 0.f, -1.f, clr_r, 1.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, len, -len, 0.f, 0.f, -1.f, clr_r, 0.f, 0.f));
            
            buffer->vertices_.PushBack(video::S3DVertex(len, -len, -len, 1.f, 0.f, 0.f, clr_g, 0.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, -len, len, 1.f, 0.f, 0.f, clr_g, 1.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, len, len, 1.f, 0.f, 0.f, clr_g, 1.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, len, -len, 1.f, 0.f, 0.f, clr_g, 0.f, 0.f));

            buffer->vertices_.PushBack(video::S3DVertex(len, -len, len, 0.f, 0.f, 1.f, clr_r, 0.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, -len, len, 0.f, 0.f, 1.f, clr_r, 1.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, len, len, 0.f, 0.f, 1.f, clr_r, 1.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, len, len, 0.f, 0.f, 1.f, clr_r, 0.f, 0.f));

            buffer->vertices_.PushBack(video::S3DVertex(-len, -len, len, -1.f, 0.f, 0.f, clr_g, 0.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, -len, -len, -1.f, 0.f, 0.f, clr_g, 1.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, len, -len, -1.f, 0.f, 0.f, clr_g, 1.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, len, len, -1.f, 0.f, 0.f, clr_g, 0.f, 0.f));

            buffer->vertices_.PushBack(video::S3DVertex(-len, len, -len, 0.f, 1.f, 0.f, clr_b, 0.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, len, -len, 0.f, 1.f, 0.f, clr_b, 1.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, len, len, 0.f, 1.f, 0.f, clr_b, 1.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, len, len, 0.f, 1.f, 0.f, clr_b, 0.f, 0.f));

            buffer->vertices_.PushBack(video::S3DVertex(-len, -len, len, 0.f, -1.f, 0.f, clr_b, 0.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, -len, len, 0.f, -1.f, 0.f, clr_b, 1.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, -len, -len, 0.f, -1.f, 0.f, clr_b, 1.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, -len, -len, 0.f, -1.f, 0.f, clr_b, 0.f, 0.f));

            buffer->indices_.Reallocate(36);

            // reallocate the barycentric buffer
            for (int i = 0; i < 6; i++)
            {
                int idx = i * 4;
                buffer->vertices_[idx].barycentric_ = core::vector3df(1.f, 0.f, 0.f);
                buffer->vertices_[idx + 1].barycentric_ = core::vector3df(0.f, 1.f, 0.f);
                buffer->vertices_[idx + 2].barycentric_ = core::vector3df(0.f, 0.f, 1.f);
                buffer->vertices_[idx + 3].barycentric_ = core::vector3df(0.f, 1.f, 0.f);
            }

            for (auto indice : indices)
            {
                buffer->indices_.PushBack(indice);
            }

            buffer->RecalculateBoundingBox();

            SMesh *tmp = new SMesh();
            tmp->AddMeshBuffer(buffer);
            mesh_ = tmp;
        }
    } // end namespace scene
} // end namespace kong