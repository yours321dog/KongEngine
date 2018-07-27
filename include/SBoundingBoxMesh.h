// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".
// This file is used to describe bounding box mesh

#ifndef _SBOUNDINGBOXMESH_H_
#define _SBOUNDINGBOXMESH_H_

#include "SMesh.h"
#include "CMeshBuffer.h"

namespace kong
{
    namespace scene
    {
        class SBoundingBoxMesh
        {
        public:
            SBoundingBoxMesh();
            ~SBoundingBoxMesh();

            //! Rebuild the bounding box mesh
            /* \param box The range of boudning box*/
            void RebuildBoundingBoxMesh(const core::aabbox3d<f32>& box) const;

            //! Get bounding box mesh
            IMesh *GetBoundingBoxMesh() const;

        private:
            void GenerateMesh();

            IMesh *mesh_;
        };

        inline SBoundingBoxMesh::SBoundingBoxMesh()
            : mesh_(nullptr)
        {
            GenerateMesh();
        }

        inline SBoundingBoxMesh::~SBoundingBoxMesh()
        {
            delete mesh_;
        }

        inline void SBoundingBoxMesh::RebuildBoundingBoxMesh(const core::aabbox3d<f32>& box) const
        {
            f32 left = box.MinEdge.x_;
            f32 right = box.MaxEdge.x_;
            f32 bottom = box.MinEdge.y_;
            f32 top = box.MaxEdge.y_;
            f32 front = box.MinEdge.z_;
            f32 back = box.MaxEdge.z_;

            video::S3DVertex *vertices = static_cast<video::S3DVertex *>(mesh_->GetMeshBuffer(0)->GetVertices());
            vertices[0].pos_ = core::vector3df(left, bottom, front);
            vertices[1].pos_ = core::vector3df(right, bottom, front);
            vertices[2].pos_ = core::vector3df(right, top, front);
            vertices[3].pos_ = core::vector3df(left, top, front);

            vertices[4].pos_ = core::vector3df(right, bottom, front);
            vertices[5].pos_ = core::vector3df(right, bottom, back);
            vertices[6].pos_ = core::vector3df(right, top, back);
            vertices[7].pos_ = core::vector3df(right, top, front);

            vertices[8].pos_ = core::vector3df(right, bottom, back);
            vertices[9].pos_ = core::vector3df(left, bottom, back);
            vertices[10].pos_ = core::vector3df(left, top, back);
            vertices[11].pos_ = core::vector3df(right, top, back);

            vertices[12].pos_ = core::vector3df(left, bottom, back);
            vertices[13].pos_ = core::vector3df(left, bottom, front);
            vertices[14].pos_ = core::vector3df(left, top, front);
            vertices[15].pos_ = core::vector3df(left, top, back);

            vertices[16].pos_ = core::vector3df(left, top, front);
            vertices[17].pos_ = core::vector3df(right, top, front);
            vertices[18].pos_ = core::vector3df(right, top, back);
            vertices[19].pos_ = core::vector3df(left, top, back);

            vertices[20].pos_ = core::vector3df(left, bottom, back);
            vertices[21].pos_ = core::vector3df(right, bottom, back);
            vertices[22].pos_ = core::vector3df(right, bottom, front);
            vertices[23].pos_ = core::vector3df(left, bottom, front);
        }

        inline IMesh* SBoundingBoxMesh::GetBoundingBoxMesh() const
        {
            return mesh_;
        }

        inline void SBoundingBoxMesh::GenerateMesh()
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

            const f32 len = 1.f / 2.f;

            buffer->vertices_.PushBack(video::S3DVertex(-len, -len, -len, 0.f, 0.f, -1.f, clr_r, 0.f, 1.f));
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

            SMesh *tmp = new SMesh();
            tmp->AddMeshBuffer(buffer);
            mesh_ = tmp;
        }
    } // end namespace scene
} // end namespace kong

#endif