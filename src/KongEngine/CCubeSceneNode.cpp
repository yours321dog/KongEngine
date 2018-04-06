// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "CCubeSceneNode.h"
#include "SMesh.h"
#include "CMeshBuffer.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"

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

        void CCubeSceneNode::Render()
        {
            video::IVideoDriver *driver = scene_manager_->GetVideoDriver();
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
            delete mesh_;
            mesh_ = mesh;
        }

        IMesh* CCubeSceneNode::GetMesh()
        {
            return mesh_;
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

            const video::SColor clr = video::SColor(255, 255, 255, 255);

            buffer->vertices_.Resize(24);

            f32 len = size_ / 2.f;

            buffer->vertices_.PushBack(video::S3DVertex(-len, -len, -len, 0.f, 0.f, -1.f, clr, 0.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, -len, -len, 0.f, 0.f, -1.f, clr, 1.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, len, -len, 0.f, 0.f, -1.f, clr, 1.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, len, -len, 0.f, 0.f, -1.f, clr, 0.f, 1.f));
            
            buffer->vertices_.PushBack(video::S3DVertex(len, -len, -len, 0.f, 0.f, -1.f, clr, 0.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, -len, len, 0.f, 0.f, -1.f, clr, 1.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, len, len, 0.f, 0.f, -1.f, clr, 1.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, len, -len, 0.f, 0.f, -1.f, clr, 0.f, 1.f));

            buffer->vertices_.PushBack(video::S3DVertex(len, len, len, 0.f, 0.f, -1.f, clr, 0.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, len, len, 0.f, 0.f, -1.f, clr, 1.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, -len, len, 0.f, 0.f, -1.f, clr, 1.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, -len, len, 0.f, 0.f, -1.f, clr, 0.f, 1.f));

            buffer->vertices_.PushBack(video::S3DVertex(-len, -len, len, 0.f, 0.f, -1.f, clr, 0.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, -len, -len, 0.f, 0.f, -1.f, clr, 1.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, len, -len, 0.f, 0.f, -1.f, clr, 1.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, len, len, 0.f, 0.f, -1.f, clr, 0.f, 1.f));

            buffer->vertices_.PushBack(video::S3DVertex(-len, len, -len, 0.f, 0.f, -1.f, clr, 0.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, len, -len, 0.f, 0.f, -1.f, clr, 1.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, len, len, 0.f, 0.f, -1.f, clr, 1.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, len, len, 0.f, 0.f, -1.f, clr, 0.f, 1.f));

            buffer->vertices_.PushBack(video::S3DVertex(len, -len, len, 0.f, 0.f, -1.f, clr, 0.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, -len, len, 0.f, 0.f, -1.f, clr, 1.f, 0.f));
            buffer->vertices_.PushBack(video::S3DVertex(-len, -len, -len, 0.f, 0.f, -1.f, clr, 1.f, 1.f));
            buffer->vertices_.PushBack(video::S3DVertex(len, -len, -len, 0.f, 0.f, -1.f, clr, 0.f, 1.f));

            buffer->indices_.Resize(36);

            for (auto indice : indices)
            {
                buffer->indices_.PushBack(indice);
            }

            SMesh *tmp = new SMesh();
            tmp->AddMeshBuffer(buffer);
            mesh_ = tmp;
        }
    } // end namespace scene
} // end namespace kong