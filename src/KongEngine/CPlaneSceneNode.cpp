// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "CPlaneSceneNode.h"
#include "SMesh.h"
#include "CMeshBuffer.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"

namespace kong
{
    namespace scene
    {
        CPlaneSceneNode::CPlaneSceneNode(f32 size, ISceneNode* parent, ISceneManager* mgr, s32 id,
            const core::Vector3Df& position, const core::Vector3Df& rotation, const core::Vector3Df& scale)
            : IMeshSceneNode(parent, mgr, id, position, rotation, scale), size_(size)
        {
            GenerateMesh();
        }

        CPlaneSceneNode::~CPlaneSceneNode()
        {
            delete mesh_;
        }

        const core::aabbox3d<f32>& CPlaneSceneNode::GetBoundingBox() const
        {
            return mesh_->GetMeshBuffer(0)->GetBoundingBox();
        }

        void CPlaneSceneNode::Render()
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
        }

        video::SMaterial& CPlaneSceneNode::GetMaterial(u32 num)
        {
            return mesh_->GetMeshBuffer(num)->GetMaterial();
        }

        u32 CPlaneSceneNode::GetMaterialCount() const
        {
            return mesh_->GetMeshBufferCount();
        }

        void CPlaneSceneNode::SetMesh(IMesh* mesh)
        {
            //delete mesh_;
            mesh_ = mesh;
        }

        IMesh* CPlaneSceneNode::GetMesh()
        {
            return mesh_;
        }

        void CPlaneSceneNode::OnRegisterSceneNode()
        {
            if (scene_manager_ != nullptr && IsVisible())
            {
                scene_manager_->RegisterNodeForRendering(this, ESNRP_SOLID);
            }
            ISceneNode::OnRegisterSceneNode();
        }

        void CPlaneSceneNode::GenerateMesh()
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

            const video::SColor clr_b = video::SColor(255, 128, 128, 128);
            const video::SColor clr_t = video::SColor(255, 150, 150, 150);

            buffer->vertices_.Reallocate(24);

            const f32 len = size_ / 2.f;
            const f32 height = 0.005f;

            buffer->vertices_.PushBack(video::S3DVertex(-len, -len, -height, 0.f, 0.f, -1.f, clr_b, 0.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, -len, -height, 0.f, 0.f, -1.f, clr_b, 1.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, len, -height, 0.f, 0.f, -1.f, clr_t, 1.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, len, -height, 0.f, 0.f, -1.f, clr_t, 0.f, 0.f));

            buffer->vertices_.PushBack(video::S3DVertex(len, -len, -height, 1.f, 0.f, 0.f, clr_b, 0.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, -len, height, 1.f, 0.f, 0.f, clr_b, 1.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, len, height, 1.f, 0.f, 0.f, clr_t, 1.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, len, -height, 1.f, 0.f, 0.f, clr_t, 0.f, 0.f));

            buffer->vertices_.PushBack(video::S3DVertex(len, -len, height, 0.f, 0.f, 1.f, clr_b, 0.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, -len, height, 0.f, 0.f, 1.f, clr_b, 1.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, len, height, 0.f, 0.f, 1.f, clr_t, 1.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, len, height, 0.f, 0.f, 1.f, clr_t, 0.f, 0.f));

            buffer->vertices_.PushBack(video::S3DVertex(-len, -len, height, -1.f, 0.f, 0.f, clr_b, 0.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, -len, -height, -1.f, 0.f, 0.f, clr_b, 1.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, len, -height, -1.f, 0.f, 0.f, clr_t, 1.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, len, height, -1.f, 0.f, 0.f, clr_t, 0.f, 0.f));

            buffer->vertices_.PushBack(video::S3DVertex(-len, len, -height, 0.f, 1.f, 0.f, clr_t, 0.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, len, -height, 0.f, 1.f, 0.f, clr_t, 1.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, len, height, 0.f, 1.f, 0.f, clr_t, 1.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, len, height, 0.f, 1.f, 0.f, clr_t, 0.f, 0.f));

            buffer->vertices_.PushBack(video::S3DVertex(-len, -len, height, 0.f, -1.f, 0.f, clr_b, 0.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, -len, height, 0.f, -1.f, 0.f, clr_b, 1.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, -len, -height, 0.f, -1.f, 0.f, clr_b, 1.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, -len, -height, 0.f, -1.f, 0.f, clr_b, 0.f, 0.f));

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