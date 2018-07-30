// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CMeshManipulator.h"
#include "SMesh.h"
#include "CMeshBuffer.h"
#include "SAnimatedMesh.h"
#include "os.h"
#include "Map.h"
#include "KongMath.h"

namespace kong
{
    namespace scene
    {

        static inline core::vector3df getAngleWeight(const core::vector3df& v1,
            const core::vector3df& v2,
            const core::vector3df& v3)
        {
            // Calculate this triangle's weight for each of its three vertices
            // start by calculating the lengths of its sides
            const f32 a = v2.GetDistanceFromSQ(v3);
            const f32 asqrt = sqrtf(a);
            const f32 b = v1.GetDistanceFromSQ(v3);
            const f32 bsqrt = sqrtf(b);
            const f32 c = v1.GetDistanceFromSQ(v2);
            const f32 csqrt = sqrtf(c);

            // use them to find the angle at each vertex
            return core::vector3df(
                acosf((b + c - a) / (2.f * bsqrt * csqrt)),
                acosf((-b + c + a) / (2.f * asqrt * csqrt)),
                acosf((b - c + a) / (2.f * bsqrt * asqrt)));
        }


        //! Flips the direction of surfaces. Changes backfacing triangles to frontfacing
        //! triangles and vice versa.
        //! \param mesh: Mesh on which the operation is performed.
        void CMeshManipulator::flipSurfaces(scene::IMesh* mesh) const
        {
            if (!mesh)
                return;

            const u32 bcount = mesh->GetMeshBufferCount();
            for (u32 b = 0; b<bcount; ++b)
            {
                IMeshBuffer* buffer = mesh->GetMeshBuffer(b);
                const u32 idxcnt = buffer->GetIndexCount();
                if (buffer->GetIndexType() == video::EIT_16BIT)
                {
                    u16* idx = buffer->GetIndices();
                    for (u32 i = 0; i<idxcnt; i += 3)
                    {
                        const u16 tmp = idx[i + 1];
                        idx[i + 1] = idx[i + 2];
                        idx[i + 2] = tmp;
                    }
                }
                else
                {
                    u32* idx = reinterpret_cast<u32*>(buffer->GetIndices());
                    for (u32 i = 0; i<idxcnt; i += 3)
                    {
                        const u32 tmp = idx[i + 1];
                        idx[i + 1] = idx[i + 2];
                        idx[i + 2] = tmp;
                    }
                }
            }
        }


        namespace
        {
            template <typename T>
            void recalculateNormalsT(IMeshBuffer* buffer, bool smooth, bool angleWeighted)
            {
                const u32 vtxcnt = buffer->GetVertexCount();
                const u32 idxcnt = buffer->GetIndexCount();
                const T* idx = reinterpret_cast<T*>(buffer->GetIndices());

                if (!smooth)
                {
                    for (u32 i = 0; i<idxcnt; i += 3)
                    {
                        const core::vector3df& v1 = buffer->GetPosition(idx[i + 0]);
                        const core::vector3df& v2 = buffer->GetPosition(idx[i + 1]);
                        const core::vector3df& v3 = buffer->GetPosition(idx[i + 2]);
                        const core::vector3df normal = core::plane3d<f32>(v1, v2, v3).Normal;
                        buffer->GetNormal(idx[i + 0]) = normal;
                        buffer->GetNormal(idx[i + 1]) = normal;
                        buffer->GetNormal(idx[i + 2]) = normal;
                    }
                }
                else
                {
                    u32 i;

                    for (i = 0; i != vtxcnt; ++i)
                        buffer->GetNormal(i).Set(0.f, 0.f, 0.f);

                    for (i = 0; i<idxcnt; i += 3)
                    {
                        const core::vector3df& v1 = buffer->GetPosition(idx[i + 0]);
                        const core::vector3df& v2 = buffer->GetPosition(idx[i + 1]);
                        const core::vector3df& v3 = buffer->GetPosition(idx[i + 2]);
                        const core::vector3df normal = core::plane3d<f32>(v1, v2, v3).Normal;

                        core::vector3df weight(1.f, 1.f, 1.f);
                        if (angleWeighted)
                            weight = kong::scene::getAngleWeight(v1, v2, v3); // writing irr::scene:: necessary for borland

                        buffer->GetNormal(idx[i + 0]) += normal * weight.x_;
                        buffer->GetNormal(idx[i + 1]) += normal * weight.y_;
                        buffer->GetNormal(idx[i + 2]) += normal * weight.z_;
                    }

                    for (i = 0; i != vtxcnt; ++i)
                        buffer->GetNormal(i).Normalize();
                }
            }
        }


        //! Recalculates all normals of the mesh buffer.
        /** \param buffer: Mesh buffer on which the operation is performed. */
        void CMeshManipulator::recalculateNormals(IMeshBuffer* buffer, bool smooth, bool angleWeighted) const
        {
            if (!buffer)
                return;

            if (buffer->GetIndexType() == video::EIT_16BIT)
                recalculateNormalsT<u16>(buffer, smooth, angleWeighted);
            else
                recalculateNormalsT<u32>(buffer, smooth, angleWeighted);
        }


        //! Recalculates all normals of the mesh.
        //! \param mesh: Mesh on which the operation is performed.
        void CMeshManipulator::recalculateNormals(scene::IMesh* mesh, bool smooth, bool angleWeighted) const
        {
            if (!mesh)
                return;

            const u32 bcount = mesh->GetMeshBufferCount();
            for (u32 b = 0; b<bcount; ++b)
                recalculateNormals(mesh->GetMeshBuffer(b), smooth, angleWeighted);
        }


        namespace
        {
            void calculateTangents(
                core::vector3df& normal,
                core::vector3df& tangent,
                core::vector3df& binormal,
                const core::vector3df& vt1, const core::vector3df& vt2, const core::vector3df& vt3, // vertices
                const core::vector2df& tc1, const core::vector2df& tc2, const core::vector2df& tc3) // texture coords
            {
                // choose one of them:
                //#define USE_NVIDIA_GLH_VERSION // use version used by nvidia in glh headers
#define USE_IRR_VERSION

#ifdef USE_IRR_VERSION

                core::vector3df v1 = vt1 - vt2;
                core::vector3df v2 = vt3 - vt1;
                normal = v2.CrossProduct(v1);
                normal.Normalize();

                // binormal

                f32 deltaX1 = tc1.x_ - tc2.x_;
                f32 deltaX2 = tc3.x_ - tc1.x_;
                binormal = (v1 * deltaX2) - (v2 * deltaX1);
                binormal.Normalize();

                // tangent

                f32 deltaY1 = tc1.y_ - tc2.y_;
                f32 deltaY2 = tc3.y_ - tc1.y_;
                tangent = (v1 * deltaY2) - (v2 * deltaY1);
                tangent.Normalize();

                // adjust

                core::vector3df txb = tangent.CrossProduct(binormal);
                if (txb.DotProduct(normal) < 0.0f)
                {
                    tangent *= -1.0f;
                    //binormal *= -1.0f;
                }

#endif // USE_IRR_VERSION

#ifdef USE_NVIDIA_GLH_VERSION

                tangent.Set(0, 0, 0);
                binormal.Set(0, 0, 0);

                core::vector3df v1(vt2.x_ - vt1.x_, tc2.x_ - tc1.x_, tc2.y_ - tc1.y_);
                core::vector3df v2(vt3.x_ - vt1.x_, tc3.x_ - tc1.x_, tc3.y_ - tc1.y_);

                core::vector3df txb = v1.CrossProduct(v2);
                if (!core::iszero(txb.x_))
                {
                    tangent.x_ = -txb.y_ / txb.x_;
                    binormal.x_ = -txb.z_ / txb.x_;
                }

                v1.x_ = vt2.y_ - vt1.y_;
                v2.x_ = vt3.y_ - vt1.y_;
                txb = v1.CrossProduct(v2);

                if (!core::iszero(txb.x_))
                {
                    tangent.y_ = -txb.y_ / txb.x_;
                    binormal.y_ = -txb.z_ / txb.x_;
                }

                v1.x_ = vt2.z_ - vt1.z_;
                v2.x_ = vt3.z_ - vt1.z_;
                txb = v1.CrossProduct(v2);

                if (!core::iszero(txb.x_))
                {
                    tangent.z_ = -txb.y_ / txb.x_;
                    binormal.z_ = -txb.z_ / txb.x_;
                }

                tangent.Normalize();
                binormal.Normalize();

                normal = tangent.CrossProduct(binormal);
                normal.Normalize();

                binormal = tangent.CrossProduct(normal);
                binormal.Normalize();

                core::plane3d<f32> pl(vt1, vt2, vt3);

                if (normal.dotProduct(pl.normal_) < 0.0f)
                    normal *= -1.0f;

#endif // USE_NVIDIA_GLH_VERSION
            }


            //! Recalculates tangents for a tangent mesh buffer
            template <typename T>
            void recalculateTangentsT(IMeshBuffer* buffer, bool recalculateNormals, bool smooth, bool angleWeighted)
            {

                if (!buffer || (buffer->GetVertexType() != video::EVT_TANGENTS))
                    return;

#define TANGENT_KONG_VERSION

#ifdef TANGENT_IRR_VERSION
                const u32 vtxCnt = buffer->GetVertexCount();
                const u32 idxCnt = buffer->GetIndexCount();

                T* idx = reinterpret_cast<T*>(buffer->GetIndices());
                video::S3DVertexTangents* v =
                    (video::S3DVertexTangents*)buffer->GetVertices();

                if (smooth)
                {
                    u32 i;

                    for (i = 0; i != vtxCnt; ++i)
                    {
                        if (recalculateNormals)
                            v[i].normal_.Set(0.f, 0.f, 0.f);
                        v[i].tangent_.Set(0.f, 0.f, 0.f);
                        v[i].binormal_.Set(0.f, 0.f, 0.f);
                    }

                    //Each vertex gets the sum of the tangents and binormals from the faces around it
                    for (i = 0; i<idxCnt; i += 3)
                    {
                        // if this triangle is degenerate, skip it!
                        if (v[idx[i + 0]].pos_ == v[idx[i + 1]].pos_ ||
                            v[idx[i + 0]].pos_ == v[idx[i + 2]].pos_ ||
                            v[idx[i + 1]].pos_ == v[idx[i + 2]].pos_
                            /*||
                            v[idx[i+0]].texcoord_ == v[idx[i+1]].texcoord_ ||
                            v[idx[i+0]].texcoord_ == v[idx[i+2]].texcoord_ ||
                            v[idx[i+1]].texcoord_ == v[idx[i+2]].texcoord_ */
                            )
                            continue;

                        //Angle-weighted normals look better, but are slightly more CPU intensive to calculate
                        core::vector3df weight(1.f, 1.f, 1.f);
                        if (angleWeighted)
                            weight = kong::scene::getAngleWeight(v[i + 0].pos_, v[i + 1].pos_, v[i + 2].pos_);	// writing irr::scene:: necessary for borland
                        core::vector3df localNormal;
                        core::vector3df localTangent;
                        core::vector3df localBinormal;

                        calculateTangents(
                            localNormal,
                            localTangent,
                            localBinormal,
                            v[idx[i + 0]].pos_,
                            v[idx[i + 1]].pos_,
                            v[idx[i + 2]].pos_,
                            v[idx[i + 0]].texcoord_,
                            v[idx[i + 1]].texcoord_,
                            v[idx[i + 2]].texcoord_);

                        if (recalculateNormals)
                            v[idx[i + 0]].normal_ += localNormal * weight.x_;
                        v[idx[i + 0]].tangent_ += localTangent * weight.x_;
                        v[idx[i + 0]].binormal_ += localBinormal * weight.x_;

                        calculateTangents(
                            localNormal,
                            localTangent,
                            localBinormal,
                            v[idx[i + 1]].pos_,
                            v[idx[i + 2]].pos_,
                            v[idx[i + 0]].pos_,
                            v[idx[i + 1]].texcoord_,
                            v[idx[i + 2]].texcoord_,
                            v[idx[i + 0]].texcoord_);

                        if (recalculateNormals)
                            v[idx[i + 1]].normal_ += localNormal * weight.y_;
                        v[idx[i + 1]].tangent_ += localTangent * weight.y_;
                        v[idx[i + 1]].binormal_ += localBinormal * weight.y_;

                        calculateTangents(
                            localNormal,
                            localTangent,
                            localBinormal,
                            v[idx[i + 2]].pos_,
                            v[idx[i + 0]].pos_,
                            v[idx[i + 1]].pos_,
                            v[idx[i + 2]].texcoord_,
                            v[idx[i + 0]].texcoord_,
                            v[idx[i + 1]].texcoord_);

                        if (recalculateNormals)
                            v[idx[i + 2]].normal_ += localNormal * weight.z_;
                        v[idx[i + 2]].tangent_ += localTangent * weight.z_;
                        v[idx[i + 2]].binormal_ += localBinormal * weight.z_;
                    }

                    // Normalize the tangents and binormals
                    if (recalculateNormals)
                    {
                        for (i = 0; i != vtxCnt; ++i)
                            v[i].normal_.Normalize();
                    }
                    for (i = 0; i != vtxCnt; ++i)
                    {
                        v[i].tangent_.Normalize();
                        v[i].binormal_.Normalize();
                    }
                }
                else
                {
                    core::vector3df localNormal;
                    for (u32 i = 0; i<idxCnt; i += 3)
                    {
                        calculateTangents(
                            localNormal,
                            v[idx[i + 0]].tangent_,
                            v[idx[i + 0]].binormal_,
                            v[idx[i + 0]].pos_,
                            v[idx[i + 1]].pos_,
                            v[idx[i + 2]].pos_,
                            v[idx[i + 0]].texcoord_,
                            v[idx[i + 1]].texcoord_,
                            v[idx[i + 2]].texcoord_);
                        if (recalculateNormals)
                            v[idx[i + 0]].normal_ = localNormal;

                        calculateTangents(
                            localNormal,
                            v[idx[i + 1]].tangent_,
                            v[idx[i + 1]].binormal_,
                            v[idx[i + 1]].pos_,
                            v[idx[i + 2]].pos_,
                            v[idx[i + 0]].pos_,
                            v[idx[i + 1]].texcoord_,
                            v[idx[i + 2]].texcoord_,
                            v[idx[i + 0]].texcoord_);
                        if (recalculateNormals)
                            v[idx[i + 1]].normal_ = localNormal;

                        calculateTangents(
                            localNormal,
                            v[idx[i + 2]].tangent_,
                            v[idx[i + 2]].binormal_,
                            v[idx[i + 2]].pos_,
                            v[idx[i + 0]].pos_,
                            v[idx[i + 1]].pos_,
                            v[idx[i + 2]].texcoord_,
                            v[idx[i + 0]].texcoord_,
                            v[idx[i + 1]].texcoord_);
                        if (recalculateNormals)
                            v[idx[i + 2]].normal_ = localNormal;
                    }
                }
#endif

#ifdef TANGENT_KONG_VERSION
                const u32 vtxCnt = buffer->GetVertexCount();
                const u32 idxCnt = buffer->GetIndexCount();

                T* idx = reinterpret_cast<T*>(buffer->GetIndices());
                video::S3DVertexTangents* v = static_cast<video::S3DVertexTangents*>(buffer->GetVertices());

                for (u32 i = 0; i < idxCnt; i += 3)
                {
                    video::S3DVertexTangents& v0 = v[idx[i]];
                    video::S3DVertexTangents& v1 = v[idx[i + 1]];
                    video::S3DVertexTangents& v2 = v[idx[i + 2]];

                    core::Vector3Df edge1 = v1.pos_ - v0.pos_;
                    core::Vector3Df edge2 = v2.pos_ - v0.pos_;

                    f32 delta_u1 = v1.texcoord_.x_ - v0.texcoord_.x_;
                    f32 delta_v1 = v1.texcoord_.y_ - v0.texcoord_.y_;
                    f32 delta_u2 = v2.texcoord_.x_ - v0.texcoord_.x_;
                    f32 delta_v2 = v2.texcoord_.y_ - v0.texcoord_.y_;

                    f32 f = 1.0f / (delta_u1 * delta_v2 - delta_u2 * delta_v1);

                    core::Vector3Df tangent, binormal;

                    tangent.x_ = f * (delta_v2 * edge1.x_ - delta_v1 * edge2.x_);
                    tangent.y_ = f * (delta_v2 * edge1.y_ - delta_v1 * edge2.y_);
                    tangent.z_ = f * (delta_v2 * edge1.z_ - delta_v1 * edge2.z_);

                    binormal.x_ = f * (-delta_u2 * edge1.x_ + delta_u1 * edge2.x_);
                    binormal.y_ = f * (-delta_u2 * edge1.y_ + delta_u1 * edge2.y_);
                    binormal.z_ = f * (-delta_u2 * edge1.z_ + delta_u1 * edge2.z_);

                    v0.tangent_ += tangent;
                    v1.tangent_ += tangent;
                    v2.tangent_ += tangent;

                    v0.binormal_ += binormal;
                    v1.binormal_ += binormal;
                    v2.binormal_ += binormal;
                }

                for (u32 i = 0; i < vtxCnt; i++)
                {
                    v[i].tangent_.Normalize();
                    v[i].binormal_.Normalize();
                }
#endif
            }
        }


        //! Recalculates tangents for a tangent mesh buffer
        void CMeshManipulator::recalculateTangents(IMeshBuffer* buffer, bool recalculateNormals, bool smooth, bool angleWeighted) const
        {
            if (buffer && (buffer->GetVertexType() == video::EVT_TANGENTS))
            {
                if (buffer->GetIndexType() == video::EIT_16BIT)
                    recalculateTangentsT<u16>(buffer, recalculateNormals, smooth, angleWeighted);
                else
                    recalculateTangentsT<u32>(buffer, recalculateNormals, smooth, angleWeighted);
            }
        }


        //! Recalculates tangents for all tangent mesh buffers
        void CMeshManipulator::recalculateTangents(IMesh* mesh, bool recalculateNormals, bool smooth, bool angleWeighted) const
        {
            if (!mesh)
                return;

            const u32 meshBufferCount = mesh->GetMeshBufferCount();
            for (u32 b = 0; b<meshBufferCount; ++b)
            {
                recalculateTangents(mesh->GetMeshBuffer(b), recalculateNormals, smooth, angleWeighted);
            }
        }


        namespace
        {
            //! Creates a planar texture mapping on the meshbuffer
            template<typename T>
            void makePlanarTextureMappingT(scene::IMeshBuffer* buffer, f32 resolution)
            {
                u32 idxcnt = buffer->GetIndexCount();
                T* idx = reinterpret_cast<T*>(buffer->GetIndices());

                for (u32 i = 0; i<idxcnt; i += 3)
                {
                    core::plane3df p(buffer->GetPosition(idx[i + 0]), buffer->GetPosition(idx[i + 1]), buffer->GetPosition(idx[i + 2]));
                    p.Normal.x_ = fabsf(p.Normal.x_);
                    p.Normal.y_ = fabsf(p.Normal.y_);
                    p.Normal.z_ = fabsf(p.Normal.z_);
                    // calculate planar mapping worldspace coordinates

                    if (p.Normal.x_ > p.Normal.y_ && p.Normal.x_ > p.Normal.z_)
                    {
                        for (u32 o = 0; o != 3; ++o)
                        {
                            buffer->GetTCoords(idx[i + o]).x_ = buffer->GetPosition(idx[i + o]).y_ * resolution;
                            buffer->GetTCoords(idx[i + o]).y_ = buffer->GetPosition(idx[i + o]).z_ * resolution;
                        }
                    }
                    else
                        if (p.Normal.y_ > p.Normal.x_ && p.Normal.y_ > p.Normal.z_)
                        {
                            for (u32 o = 0; o != 3; ++o)
                            {
                                buffer->GetTCoords(idx[i + o]).x_ = buffer->GetPosition(idx[i + o]).x_ * resolution;
                                buffer->GetTCoords(idx[i + o]).y_ = buffer->GetPosition(idx[i + o]).z_ * resolution;
                            }
                        }
                        else
                        {
                            for (u32 o = 0; o != 3; ++o)
                            {
                                buffer->GetTCoords(idx[i + o]).x_ = buffer->GetPosition(idx[i + o]).x_ * resolution;
                                buffer->GetTCoords(idx[i + o]).y_ = buffer->GetPosition(idx[i + o]).y_ * resolution;
                            }
                        }
                }
            }
        }


        //! Creates a planar texture mapping on the meshbuffer
        void CMeshManipulator::makePlanarTextureMapping(scene::IMeshBuffer* buffer, f32 resolution) const
        {
            if (!buffer)
                return;

            if (buffer->GetIndexType() == video::EIT_16BIT)
                makePlanarTextureMappingT<u16>(buffer, resolution);
            else
                makePlanarTextureMappingT<u32>(buffer, resolution);
        }


        //! Creates a planar texture mapping on the mesh
        void CMeshManipulator::makePlanarTextureMapping(scene::IMesh* mesh, f32 resolution) const
        {
            if (!mesh)
                return;

            const u32 bcount = mesh->GetMeshBufferCount();
            for (u32 b = 0; b<bcount; ++b)
            {
                makePlanarTextureMapping(mesh->GetMeshBuffer(b), resolution);
            }
        }


        namespace
        {
            //! Creates a planar texture mapping on the meshbuffer
            template <typename T>
            void makePlanarTextureMappingT(scene::IMeshBuffer* buffer, f32 resolutionS, f32 resolutionT, u8 axis, const core::vector3df& offset)
            {
                u32 idxcnt = buffer->GetIndexCount();
                T* idx = reinterpret_cast<T*>(buffer->GetIndices());

                for (u32 i = 0; i<idxcnt; i += 3)
                {
                    // calculate planar mapping worldspace coordinates
                    if (axis == 0)
                    {
                        for (u32 o = 0; o != 3; ++o)
                        {
                            buffer->GetTCoords(idx[i + o]).x_ = 0.5f + (buffer->GetPosition(idx[i + o]).z_ + offset.z_) * resolutionS;
                            buffer->GetTCoords(idx[i + o]).y_ = 0.5f - (buffer->GetPosition(idx[i + o]).y_ + offset.y_) * resolutionT;
                        }
                    }
                    else if (axis == 1)
                    {
                        for (u32 o = 0; o != 3; ++o)
                        {
                            buffer->GetTCoords(idx[i + o]).x_ = 0.5f + (buffer->GetPosition(idx[i + o]).x_ + offset.x_) * resolutionS;
                            buffer->GetTCoords(idx[i + o]).y_ = 1.f - (buffer->GetPosition(idx[i + o]).z_ + offset.z_) * resolutionT;
                        }
                    }
                    else if (axis == 2)
                    {
                        for (u32 o = 0; o != 3; ++o)
                        {
                            buffer->GetTCoords(idx[i + o]).x_ = 0.5f + (buffer->GetPosition(idx[i + o]).x_ + offset.x_) * resolutionS;
                            buffer->GetTCoords(idx[i + o]).y_ = 0.5f - (buffer->GetPosition(idx[i + o]).y_ + offset.y_) * resolutionT;
                        }
                    }
                }
            }
        }


        //! Creates a planar texture mapping on the meshbuffer
        void CMeshManipulator::makePlanarTextureMapping(scene::IMeshBuffer* buffer, f32 resolutionS, f32 resolutionT, u8 axis, const core::vector3df& offset) const
        {
            if (!buffer)
                return;

            if (buffer->GetIndexType() == video::EIT_16BIT)
                makePlanarTextureMappingT<u16>(buffer, resolutionS, resolutionT, axis, offset);
            else
                makePlanarTextureMappingT<u32>(buffer, resolutionS, resolutionT, axis, offset);
        }


        //! Creates a planar texture mapping on the mesh
        void CMeshManipulator::makePlanarTextureMapping(scene::IMesh* mesh, f32 resolutionS, f32 resolutionT, u8 axis, const core::vector3df& offset) const
        {
            if (!mesh)
                return;

            const u32 bcount = mesh->GetMeshBufferCount();
            for (u32 b = 0; b<bcount; ++b)
            {
                makePlanarTextureMapping(mesh->GetMeshBuffer(b), resolutionS, resolutionT, axis, offset);
            }
        }


        //! Clones a static IMesh into a modifyable SMesh.
        // not yet 32bit
        SMesh* CMeshManipulator::createMeshCopy(scene::IMesh* mesh) const
        {
            if (!mesh)
                return nullptr;

            SMesh* clone = new SMesh();

            const u32 meshBufferCount = mesh->GetMeshBufferCount();

            for (u32 b = 0; b<meshBufferCount; ++b)
            {
                const IMeshBuffer* const mb = mesh->GetMeshBuffer(b);
                switch (mb->GetVertexType())
                {
                case video::EVT_STANDARD:
                {
                    SMeshBuffer* buffer = new SMeshBuffer();
                    buffer->material_ = mb->GetMaterial();
                    const u32 vcount = mb->GetVertexCount();
                    buffer->vertices_.Reallocate(vcount);
                    video::S3DVertex* vertices = (video::S3DVertex*)mb->GetVertices();
                    for (u32 i = 0; i < vcount; ++i)
                        buffer->vertices_.PushBack(vertices[i]);
                    const u32 icount = mb->GetIndexCount();
                    buffer->indices_.Reallocate(icount);
                    const u16* indices = mb->GetIndices();
                    for (u32 i = 0; i < icount; ++i)
                        buffer->indices_.PushBack(indices[i]);
                    clone->AddMeshBuffer(buffer);
                    //buffer->drop();
                }
                break;
                case video::EVT_2TCOORDS:
                {
                    SMeshBufferLightMap* buffer = new SMeshBufferLightMap();
                    buffer->material_ = mb->GetMaterial();
                    const u32 vcount = mb->GetVertexCount();
                    buffer->vertices_.Reallocate(vcount);
                    video::S3DVertex2TCoords* vertices = (video::S3DVertex2TCoords*)mb->GetVertices();
                    for (u32 i = 0; i < vcount; ++i)
                        buffer->vertices_.PushBack(vertices[i]);
                    const u32 icount = mb->GetIndexCount();
                    buffer->indices_.Reallocate(icount);
                    const u16* indices = mb->GetIndices();
                    for (u32 i = 0; i < icount; ++i)
                        buffer->indices_.PushBack(indices[i]);
                    clone->AddMeshBuffer(buffer);
                    //buffer->drop();
                }
                break;
                case video::EVT_TANGENTS:
                {
                    SMeshBufferTangents* buffer = new SMeshBufferTangents();
                    buffer->material_ = mb->GetMaterial();
                    const u32 vcount = mb->GetVertexCount();
                    buffer->vertices_.Reallocate(vcount);
                    video::S3DVertexTangents* vertices = (video::S3DVertexTangents*)mb->GetVertices();
                    for (u32 i = 0; i < vcount; ++i)
                        buffer->vertices_.PushBack(vertices[i]);
                    const u32 icount = mb->GetIndexCount();
                    buffer->indices_.Reallocate(icount);
                    const u16* indices = mb->GetIndices();
                    for (u32 i = 0; i < icount; ++i)
                        buffer->indices_.PushBack(indices[i]);
                    clone->AddMeshBuffer(buffer);
                    //buffer->drop();
                }
                break;
                }// end switch

            }// end for all mesh buffers

            clone->bounding_box_ = mesh->GetBoundingBox();
            return clone;
        }


        //! Creates a copy of the mesh, which will only consist of unique primitives
        // not yet 32bit
        IMesh* CMeshManipulator::createMeshUniquePrimitives(IMesh* mesh) const
        {
            if (!mesh)
                return 0;

            SMesh* clone = new SMesh();

            const u32 meshBufferCount = mesh->GetMeshBufferCount();

            for (u32 b = 0; b<meshBufferCount; ++b)
            {
                const IMeshBuffer* const mb = mesh->GetMeshBuffer(b);
                const s32 idxCnt = mb->GetIndexCount();
                const u16* idx = mb->GetIndices();

                switch (mb->GetVertexType())
                {
                case video::EVT_STANDARD:
                {
                    SMeshBuffer* buffer = new SMeshBuffer();
                    buffer->material_ = mb->GetMaterial();

                    video::S3DVertex* v =
                        (video::S3DVertex*)mb->GetVertices();

                    buffer->vertices_.Reallocate(idxCnt);
                    buffer->indices_.Reallocate(idxCnt);
                    for (s32 i = 0; i<idxCnt; i += 3)
                    {
                        buffer->vertices_.PushBack(v[idx[i + 0]]);
                        buffer->vertices_.PushBack(v[idx[i + 1]]);
                        buffer->vertices_.PushBack(v[idx[i + 2]]);

                        buffer->indices_.PushBack(i + 0);
                        buffer->indices_.PushBack(i + 1);
                        buffer->indices_.PushBack(i + 2);
                    }

                    buffer->SetBoundingBox(mb->GetBoundingBox());
                    clone->AddMeshBuffer(buffer);
                    //buffer->drop();
                }
                break;
                case video::EVT_2TCOORDS:
                {
                    SMeshBufferLightMap* buffer = new SMeshBufferLightMap();
                    buffer->material_ = mb->GetMaterial();

                    video::S3DVertex2TCoords* v =
                        (video::S3DVertex2TCoords*)mb->GetVertices();

                    buffer->vertices_.Reallocate(idxCnt);
                    buffer->indices_.Reallocate(idxCnt);
                    for (s32 i = 0; i<idxCnt; i += 3)
                    {
                        buffer->vertices_.PushBack(v[idx[i + 0]]);
                        buffer->vertices_.PushBack(v[idx[i + 1]]);
                        buffer->vertices_.PushBack(v[idx[i + 2]]);

                        buffer->indices_.PushBack(i + 0);
                        buffer->indices_.PushBack(i + 1);
                        buffer->indices_.PushBack(i + 2);
                    }
                    buffer->SetBoundingBox(mb->GetBoundingBox());
                    clone->AddMeshBuffer(buffer);
                    //buffer->drop();
                }
                break;
                case video::EVT_TANGENTS:
                {
                    SMeshBufferTangents* buffer = new SMeshBufferTangents();
                    buffer->material_ = mb->GetMaterial();

                    video::S3DVertexTangents* v =
                        (video::S3DVertexTangents*)mb->GetVertices();

                    buffer->vertices_.Reallocate(idxCnt);
                    buffer->indices_.Reallocate(idxCnt);
                    for (s32 i = 0; i<idxCnt; i += 3)
                    {
                        buffer->vertices_.PushBack(v[idx[i + 0]]);
                        buffer->vertices_.PushBack(v[idx[i + 1]]);
                        buffer->vertices_.PushBack(v[idx[i + 2]]);

                        buffer->indices_.PushBack(i + 0);
                        buffer->indices_.PushBack(i + 1);
                        buffer->indices_.PushBack(i + 2);
                    }

                    buffer->SetBoundingBox(mb->GetBoundingBox());
                    clone->AddMeshBuffer(buffer);
                    //buffer->drop();
                }
                break;
                }// end switch

            }// end for all mesh buffers

            clone->bounding_box_ = mesh->GetBoundingBox();
            return clone;
        }


        //! Creates a copy of a mesh, which will have identical vertices welded together
        // not yet 32bit
        IMesh* CMeshManipulator::createMeshWelded(IMesh *mesh, f32 tolerance) const
        {
            SMesh* clone = new SMesh();
            clone->bounding_box_ = mesh->GetBoundingBox();

            core::Array<u16> redirects;

            for (u32 b = 0; b<mesh->GetMeshBufferCount(); ++b)
            {
                const IMeshBuffer* const mb = mesh->GetMeshBuffer(b);
                // reset redirect list
                redirects.Resize(mb->GetVertexCount());

                const u16* indices = 0;
                u32 indexCount = 0;
                core::Array<u16>* outIdx = 0;

                switch (mb->GetVertexType())
                {
                case video::EVT_STANDARD:
                {
                    SMeshBuffer* buffer = new SMeshBuffer();
                    buffer->bounding_box_ = mb->GetBoundingBox();
                    buffer->material_ = mb->GetMaterial();
                    clone->AddMeshBuffer(buffer);
                    //buffer->drop();

                    video::S3DVertex* v =
                        (video::S3DVertex*)mb->GetVertices();

                    u32 vertexCount = mb->GetVertexCount();

                    indices = mb->GetIndices();
                    indexCount = mb->GetIndexCount();
                    outIdx = &buffer->indices_;

                    buffer->vertices_.Reallocate(vertexCount);

                    for (u32 i = 0; i < vertexCount; ++i)
                    {
                        bool found = false;
                        for (u32 j = 0; j < i; ++j)
                        {
                            if (v[i].pos_.Equals(v[j].pos_, tolerance) &&
                                v[i].normal_.Equals(v[j].normal_, tolerance) &&
                                v[i].texcoord_.Equals(v[j].texcoord_) &&
                                (v[i].color_ == v[j].color_))
                            {
                                redirects[i] = redirects[j];
                                found = true;
                                break;
                            }
                        }
                        if (!found)
                        {
                            redirects[i] = buffer->vertices_.Size();
                            buffer->vertices_.PushBack(v[i]);
                        }
                    }

                    break;
                }
                case video::EVT_2TCOORDS:
                {
                    SMeshBufferLightMap* buffer = new SMeshBufferLightMap();
                    buffer->bounding_box_ = mb->GetBoundingBox();
                    buffer->material_ = mb->GetMaterial();
                    clone->AddMeshBuffer(buffer);
                    //buffer->drop();

                    video::S3DVertex2TCoords* v =
                        (video::S3DVertex2TCoords*)mb->GetVertices();

                    u32 vertexCount = mb->GetVertexCount();

                    indices = mb->GetIndices();
                    indexCount = mb->GetIndexCount();
                    outIdx = &buffer->indices_;

                    buffer->vertices_.Reallocate(vertexCount);

                    for (u32 i = 0; i < vertexCount; ++i)
                    {
                        bool found = false;
                        for (u32 j = 0; j < i; ++j)
                        {
                            if (v[i].pos_.Equals(v[j].pos_, tolerance) &&
                                v[i].normal_.Equals(v[j].normal_, tolerance) &&
                                v[i].texcoord_.Equals(v[j].texcoord_) &&
                                v[i].texcoord2_.Equals(v[j].texcoord2_) &&
                                (v[i].color_ == v[j].color_))
                            {
                                redirects[i] = redirects[j];
                                found = true;
                                break;
                            }
                        }
                        if (!found)
                        {
                            redirects[i] = buffer->vertices_.Size();
                            buffer->vertices_.PushBack(v[i]);
                        }
                    }
                    break;
                }
                case video::EVT_TANGENTS:
                {
                    SMeshBufferTangents* buffer = new SMeshBufferTangents();
                    buffer->bounding_box_ = mb->GetBoundingBox();
                    buffer->material_ = mb->GetMaterial();
                    clone->AddMeshBuffer(buffer);
                    //buffer->drop();

                    video::S3DVertexTangents* v =
                        (video::S3DVertexTangents*)mb->GetVertices();

                    u32 vertexCount = mb->GetVertexCount();

                    indices = mb->GetIndices();
                    indexCount = mb->GetIndexCount();
                    outIdx = &buffer->indices_;

                    buffer->vertices_.Reallocate(vertexCount);

                    for (u32 i = 0; i < vertexCount; ++i)
                    {
                        bool found = false;
                        for (u32 j = 0; j < i; ++j)
                        {
                            if (v[i].pos_.Equals(v[j].pos_, tolerance) &&
                                v[i].normal_.Equals(v[j].normal_, tolerance) &&
                                v[i].texcoord_.Equals(v[j].texcoord_) &&
                                v[i].tangent_.Equals(v[j].tangent_, tolerance) &&
                                v[i].binormal_.Equals(v[j].binormal_, tolerance) &&
                                (v[i].color_ == v[j].color_))
                            {
                                redirects[i] = redirects[j];
                                found = true;
                                break;
                            }
                        }
                        if (!found)
                        {
                            redirects[i] = buffer->vertices_.Size();
                            buffer->vertices_.PushBack(v[i]);
                        }
                    }
                    break;
                }
                default:
                    os::Printer::log("Cannot create welded mesh, vertex type unsupported", ELL_ERROR);
                    break;
                }

                // write the buffer's index list
                core::Array<u16> &Indices = *outIdx;

                Indices.Resize(indexCount);
                for (u32 i = 0; i<indexCount; ++i)
                {
                    Indices[i] = redirects[indices[i]];
                }
            }
            return clone;
        }


        //! Creates a copy of the mesh, which will only consist of S3DVertexTangents vertices.
        // not yet 32bit
        IMesh* CMeshManipulator::createMeshWithTangents(IMesh* mesh, bool recalculateNormals, bool smooth, bool angleWeighted, bool calculateTangents) const
        {
            if (!mesh)
                return 0;

            // copy mesh and fill data into SMeshBufferTangents

            SMesh* clone = new SMesh();
            const u32 meshBufferCount = mesh->GetMeshBufferCount();

            for (u32 b = 0; b<meshBufferCount; ++b)
            {
                const IMeshBuffer* const original = mesh->GetMeshBuffer(b);
                const u32 idxCnt = original->GetIndexCount();
                const u16* idx = original->GetIndices();

                SMeshBufferTangents* buffer = new SMeshBufferTangents();

                buffer->material_ = original->GetMaterial();
                buffer->vertices_.Reallocate(idxCnt);
                buffer->indices_.Reallocate(idxCnt);

                core::Map<video::S3DVertexTangents, int> vertMap;
                int vertLocation;

                // copy vertices

                const video::E_VERTEX_TYPE vType = original->GetVertexType();
                video::S3DVertexTangents vNew;
                for (u32 i = 0; i<idxCnt; ++i)
                {
                    switch (vType)
                    {
                    case video::EVT_STANDARD:
                    {
                        const video::S3DVertex* v =
                            (const video::S3DVertex*)original->GetVertices();
                        vNew = video::S3DVertexTangents(
                            v[idx[i]].pos_, v[idx[i]].normal_, v[idx[i]].color_, v[idx[i]].texcoord_);
                    }
                    break;
                    case video::EVT_2TCOORDS:
                    {
                        const video::S3DVertex2TCoords* v =
                            (const video::S3DVertex2TCoords*)original->GetVertices();
                        vNew = video::S3DVertexTangents(
                            v[idx[i]].pos_, v[idx[i]].normal_, v[idx[i]].color_, v[idx[i]].texcoord_);
                    }
                    break;
                    case video::EVT_TANGENTS:
                    {
                        const video::S3DVertexTangents* v =
                            (const video::S3DVertexTangents*)original->GetVertices();
                        vNew = v[idx[i]];
                    }
                    break;
                    }
                    core::Map<video::S3DVertexTangents, int>::Node* n = vertMap.find(vNew);
                    if (n)
                    {
                        vertLocation = n->getValue();
                    }
                    else
                    {
                        vertLocation = buffer->vertices_.Size();
                        buffer->vertices_.PushBack(vNew);
                        vertMap.insert(vNew, vertLocation);
                    }

                    // create new indices
                    buffer->indices_.PushBack(vertLocation);
                }
                buffer->RecalculateBoundingBox();

                // add new buffer
                clone->AddMeshBuffer(buffer);
                //buffer->drop();
            }

            clone->RecalculateBoundingBox();
            if (calculateTangents)
                recalculateTangents(clone, recalculateNormals, smooth, angleWeighted);

            return clone;
        }

        void CMeshManipulator::addMeshBufferhWithTangents(SMesh* clone, IMesh* mesh, bool recalculateNormals, bool smooth, bool angleWeighted, bool calculateTangents) const
        {
            if (!mesh)
                return;

            // copy mesh and fill data into SMeshBufferTangents

            const u32 meshBufferCount = mesh->GetMeshBufferCount();

            for (u32 b = 0; b<meshBufferCount; ++b)
            {
                const IMeshBuffer* const original = mesh->GetMeshBuffer(b);
                const u32 idxCnt = original->GetIndexCount();
                const u16* idx = original->GetIndices();

                SMeshBufferTangents* buffer = new SMeshBufferTangents();

                buffer->material_ = original->GetMaterial();
                buffer->vertices_.Reallocate(idxCnt);
                buffer->indices_.Reallocate(idxCnt);

                core::Map<video::S3DVertexTangents, int> vertMap;
                int vertLocation;

                // copy vertices

                const video::E_VERTEX_TYPE vType = original->GetVertexType();
                video::S3DVertexTangents vNew;
                for (u32 i = 0; i<idxCnt; ++i)
                {
                    switch (vType)
                    {
                    case video::EVT_STANDARD:
                    {
                        const video::S3DVertex* v =
                            (const video::S3DVertex*)original->GetVertices();
                        vNew = video::S3DVertexTangents(
                            v[idx[i]].pos_, v[idx[i]].normal_, v[idx[i]].color_, v[idx[i]].texcoord_);
                    }
                        break;
                    case video::EVT_2TCOORDS:
                    {
                        const video::S3DVertex2TCoords* v =
                            (const video::S3DVertex2TCoords*)original->GetVertices();
                        vNew = video::S3DVertexTangents(
                            v[idx[i]].pos_, v[idx[i]].normal_, v[idx[i]].color_, v[idx[i]].texcoord_);
                    }
                        break;
                    case video::EVT_TANGENTS:
                    {
                        const video::S3DVertexTangents* v =
                            (const video::S3DVertexTangents*)original->GetVertices();
                        vNew = v[idx[i]];
                    }
                        break;
                    }
                    core::Map<video::S3DVertexTangents, int>::Node* n = vertMap.find(vNew);
                    if (n)
                    {
                        vertLocation = n->getValue();
                    }
                    else
                    {
                        vertLocation = buffer->vertices_.Size();
                        buffer->vertices_.PushBack(vNew);
                        vertMap.insert(vNew, vertLocation);
                    }

                    // create new indices
                    buffer->indices_.PushBack(vertLocation);
                }
                buffer->RecalculateBoundingBox();

                // add new buffer
                clone->AddMeshBuffer(buffer);
                //buffer->drop();
            }

            clone->RecalculateBoundingBox();
            if (calculateTangents)
                recalculateTangents(clone, recalculateNormals, smooth, angleWeighted);
        }

        //! Creates a copy of the mesh, which will only consist of S3DVertex2TCoords vertices.
        // not yet 32bit
        IMesh* CMeshManipulator::createMeshWith2TCoords(IMesh* mesh) const
        {
            if (!mesh)
                return 0;

            // copy mesh and fill data into SMeshBufferLightMap

            SMesh* clone = new SMesh();
            const u32 meshBufferCount = mesh->GetMeshBufferCount();

            for (u32 b = 0; b<meshBufferCount; ++b)
            {
                const IMeshBuffer* const original = mesh->GetMeshBuffer(b);
                const u32 idxCnt = original->GetIndexCount();
                const u16* idx = original->GetIndices();

                SMeshBufferLightMap* buffer = new SMeshBufferLightMap();
                buffer->material_ = original->GetMaterial();
                buffer->vertices_.Reallocate(idxCnt);
                buffer->indices_.Reallocate(idxCnt);

                core::Map<video::S3DVertex2TCoords, int> vertMap;
                int vertLocation;

                // copy vertices

                const video::E_VERTEX_TYPE vType = original->GetVertexType();
                video::S3DVertex2TCoords vNew;
                for (u32 i = 0; i<idxCnt; ++i)
                {
                    switch (vType)
                    {
                    case video::EVT_STANDARD:
                    {
                        const video::S3DVertex* v =
                            (const video::S3DVertex*)original->GetVertices();
                        vNew = video::S3DVertex2TCoords(
                            v[idx[i]].pos_, v[idx[i]].normal_, v[idx[i]].color_, v[idx[i]].texcoord_, v[idx[i]].texcoord_);
                    }
                    break;
                    case video::EVT_2TCOORDS:
                    {
                        const video::S3DVertex2TCoords* v =
                            (const video::S3DVertex2TCoords*)original->GetVertices();
                        vNew = v[idx[i]];
                    }
                    break;
                    case video::EVT_TANGENTS:
                    {
                        const video::S3DVertexTangents* v =
                            (const video::S3DVertexTangents*)original->GetVertices();
                        vNew = video::S3DVertex2TCoords(
                            v[idx[i]].pos_, v[idx[i]].normal_, v[idx[i]].color_, v[idx[i]].texcoord_, v[idx[i]].texcoord_);
                    }
                    break;
                    }
                    core::Map<video::S3DVertex2TCoords, int>::Node* n = vertMap.find(vNew);
                    if (n)
                    {
                        vertLocation = n->getValue();
                    }
                    else
                    {
                        vertLocation = buffer->vertices_.Size();
                        buffer->vertices_.PushBack(vNew);
                        vertMap.insert(vNew, vertLocation);
                    }

                    // create new indices
                    buffer->indices_.PushBack(vertLocation);
                }
                buffer->RecalculateBoundingBox();

                // add new buffer
                clone->AddMeshBuffer(buffer);
                //buffer->drop();
            }

            clone->RecalculateBoundingBox();
            return clone;
        }


        //! Creates a copy of the mesh, which will only consist of S3DVertex vertices.
        // not yet 32bit
        IMesh* CMeshManipulator::createMeshWith1TCoords(IMesh* mesh) const
        {
            if (!mesh)
                return 0;

            // copy mesh and fill data into SMeshBuffer
            SMesh* clone = new SMesh();
            const u32 meshBufferCount = mesh->GetMeshBufferCount();

            for (u32 b = 0; b<meshBufferCount; ++b)
            {
                IMeshBuffer* original = mesh->GetMeshBuffer(b);
                const u32 idxCnt = original->GetIndexCount();
                const u16* idx = original->GetIndices();

                SMeshBuffer* buffer = new SMeshBuffer();
                buffer->material_ = original->GetMaterial();
                buffer->vertices_.Reallocate(idxCnt);
                buffer->indices_.Reallocate(idxCnt);

                core::Map<video::S3DVertex, int> vertMap;
                int vertLocation;

                // copy vertices
                const video::E_VERTEX_TYPE vType = original->GetVertexType();
                video::S3DVertex vNew;
                for (u32 i = 0; i<idxCnt; ++i)
                {
                    switch (vType)
                    {
                    case video::EVT_STANDARD:
                    {
                        video::S3DVertex* v =
                            (video::S3DVertex*)original->GetVertices();
                        vNew = v[idx[i]];
                    }
                    break;
                    case video::EVT_2TCOORDS:
                    {
                        video::S3DVertex2TCoords* v =
                            (video::S3DVertex2TCoords*)original->GetVertices();
                        vNew = video::S3DVertex(
                            v[idx[i]].pos_, v[idx[i]].normal_, v[idx[i]].color_, v[idx[i]].texcoord_);
                    }
                    break;
                    case video::EVT_TANGENTS:
                    {
                        video::S3DVertexTangents* v =
                            (video::S3DVertexTangents*)original->GetVertices();
                        vNew = video::S3DVertex(
                            v[idx[i]].pos_, v[idx[i]].normal_, v[idx[i]].color_, v[idx[i]].texcoord_);
                    }
                    break;
                    }
                    core::Map<video::S3DVertex, int>::Node* n = vertMap.find(vNew);
                    if (n)
                    {
                        vertLocation = n->getValue();
                    }
                    else
                    {
                        vertLocation = buffer->vertices_.Size();
                        buffer->vertices_.PushBack(vNew);
                        vertMap.insert(vNew, vertLocation);
                    }

                    // create new indices
                    buffer->indices_.PushBack(vertLocation);
                }
                buffer->RecalculateBoundingBox();
                // add new buffer
                clone->AddMeshBuffer(buffer);
                //buffer->drop();
            }

            clone->RecalculateBoundingBox();
            return clone;
        }


        //! Returns amount of polygons in mesh.
        s32 CMeshManipulator::getPolyCount(scene::IMesh* mesh) const
        {
            if (!mesh)
                return 0;

            s32 trianglecount = 0;

            for (u32 g = 0; g<mesh->GetMeshBufferCount(); ++g)
                trianglecount += mesh->GetMeshBuffer(g)->GetIndexCount() / 3;

            return trianglecount;
        }


        //! Returns amount of polygons in mesh.
        s32 CMeshManipulator::getPolyCount(scene::IAnimatedMesh* mesh) const
        {
            if (mesh && mesh->getFrameCount() != 0)
                return getPolyCount(mesh->getMesh(0));

            return 0;
        }


        //! create a new AnimatedMesh and adds the mesh to it
        IAnimatedMesh * CMeshManipulator::createAnimatedMesh(scene::IMesh* mesh, scene::E_ANIMATED_MESH_TYPE type) const
        {
            return new SAnimatedMesh(mesh, type);
        }

        namespace
        {

            struct vcache
            {
                core::Array<u32> tris;
                float score;
                s16 cachepos;
                u16 NumActiveTris;
            };

            struct tcache
            {
                u16 ind[3];
                float score;
                bool drawn;
            };

            const u16 cachesize = 32;

            float FindVertexScore(vcache *v)
            {
                const float CacheDecayPower = 1.5f;
                const float LastTriScore = 0.75f;
                const float ValenceBoostScale = 2.0f;
                const float ValenceBoostPower = 0.5f;
                const float MaxSizeVertexCache = 32.0f;

                if (v->NumActiveTris == 0)
                {
                    // No tri needs this vertex!
                    return -1.0f;
                }

                float Score = 0.0f;
                int CachePosition = v->cachepos;
                if (CachePosition < 0)
                {
                    // Vertex is not in FIFO cache - no score.
                }
                else
                {
                    if (CachePosition < 3)
                    {
                        // This vertex was used in the last triangle,
                        // so it has a fixed score.
                        Score = LastTriScore;
                    }
                    else
                    {
                        // Points for being high in the cache.
                        const float Scaler = 1.0f / (MaxSizeVertexCache - 3);
                        Score = 1.0f - (CachePosition - 3) * Scaler;
                        Score = powf(Score, CacheDecayPower);
                    }
                }

                // Bonus points for having a low number of tris still to
                // use the vert, so we get rid of lone verts quickly.
                float ValenceBoost = powf(v->NumActiveTris,
                    -ValenceBoostPower);
                Score += ValenceBoostScale * ValenceBoost;

                return Score;
            }

            /*
            A specialized LRU cache for the Forsyth algorithm.
            */

            class f_lru
            {

            public:
                f_lru(vcache *v, tcache *t) : vc(v), tc(t)
                {
                    for (u16 i = 0; i < cachesize; i++)
                    {
                        cache[i] = -1;
                    }
                }

                // Adds this vertex index and returns the highest-scoring triangle index
                u32 add(u16 vert, bool updatetris = false)
                {
                    bool found = false;

                    // Mark existing pos as empty
                    for (u16 i = 0; i < cachesize; i++)
                    {
                        if (cache[i] == vert)
                        {
                            // Move everything down
                            for (u16 j = i; j; j--)
                            {
                                cache[j] = cache[j - 1];
                            }

                            found = true;
                            break;
                        }
                    }

                    if (!found)
                    {
                        if (cache[cachesize - 1] != -1)
                            vc[cache[cachesize - 1]].cachepos = -1;

                        // Move everything down
                        for (u16 i = cachesize - 1; i; i--)
                        {
                            cache[i] = cache[i - 1];
                        }
                    }

                    cache[0] = vert;

                    u32 highest = 0;
                    float hiscore = 0;

                    if (updatetris)
                    {
                        // Update cache positions
                        for (u16 i = 0; i < cachesize; i++)
                        {
                            if (cache[i] == -1)
                                break;

                            vc[cache[i]].cachepos = i;
                            vc[cache[i]].score = FindVertexScore(&vc[cache[i]]);
                        }

                        // Update triangle scores
                        for (u16 i = 0; i < cachesize; i++)
                        {
                            if (cache[i] == -1)
                                break;

                            const u16 trisize = vc[cache[i]].tris.Size();
                            for (u16 t = 0; t < trisize; t++)
                            {
                                tcache *tri = &tc[vc[cache[i]].tris[t]];

                                tri->score =
                                    vc[tri->ind[0]].score +
                                    vc[tri->ind[1]].score +
                                    vc[tri->ind[2]].score;

                                if (tri->score > hiscore)
                                {
                                    hiscore = tri->score;
                                    highest = vc[cache[i]].tris[t];
                                }
                            }
                        }
                    }

                    return highest;
                }

            private:
                s32 cache[cachesize];
                vcache *vc;
                tcache *tc;
            };

        } // end anonymous namespace

        /**
        Vertex cache optimization according to the Forsyth paper:
        http://home.comcast.net/~tom_forsyth/papers/fast_vert_cache_opt.html

        The function is thread-safe (read: you can optimize several meshes in different threads)

        \param mesh Source mesh for the operation.  */
        IMesh* CMeshManipulator::createForsythOptimizedMesh(const IMesh *mesh) const
        {
            if (!mesh)
                return 0;

            SMesh *newmesh = new SMesh();
            newmesh->bounding_box_ = mesh->GetBoundingBox();

            const u32 mbcount = mesh->GetMeshBufferCount();

            for (u32 b = 0; b < mbcount; ++b)
            {
                const IMeshBuffer *mb = mesh->GetMeshBuffer(b);

                if (mb->GetIndexType() != video::EIT_16BIT)
                {
                    os::Printer::log("Cannot optimize a mesh with 32bit indices", ELL_ERROR);
                    //newmesh->drop();
                    delete newmesh;
                    return nullptr;
                }

                const u32 icount = mb->GetIndexCount();
                const u32 tcount = icount / 3;
                const u32 vcount = mb->GetVertexCount();
                const u16 *ind = mb->GetIndices();

                vcache *vc = new vcache[vcount];
                tcache *tc = new tcache[tcount];

                f_lru lru(vc, tc);

                // init
                for (u16 i = 0; i < vcount; i++)
                {
                    vc[i].score = 0;
                    vc[i].cachepos = -1;
                    vc[i].NumActiveTris = 0;
                }

                // First pass: count how many times a vert is used
                for (u32 i = 0; i < icount; i += 3)
                {
                    vc[ind[i]].NumActiveTris++;
                    vc[ind[i + 1]].NumActiveTris++;
                    vc[ind[i + 2]].NumActiveTris++;

                    const u32 tri_ind = i / 3;
                    tc[tri_ind].ind[0] = ind[i];
                    tc[tri_ind].ind[1] = ind[i + 1];
                    tc[tri_ind].ind[2] = ind[i + 2];
                }

                // Second pass: list of each triangle
                for (u32 i = 0; i < tcount; i++)
                {
                    vc[tc[i].ind[0]].tris.PushBack(i);
                    vc[tc[i].ind[1]].tris.PushBack(i);
                    vc[tc[i].ind[2]].tris.PushBack(i);

                    tc[i].drawn = false;
                }

                // Give initial scores
                for (u16 i = 0; i < vcount; i++)
                {
                    vc[i].score = FindVertexScore(&vc[i]);
                }
                for (u32 i = 0; i < tcount; i++)
                {
                    tc[i].score =
                        vc[tc[i].ind[0]].score +
                        vc[tc[i].ind[1]].score +
                        vc[tc[i].ind[2]].score;
                }

                switch (mb->GetVertexType())
                {
                case video::EVT_STANDARD:
                {
                    video::S3DVertex *v = (video::S3DVertex *) mb->GetVertices();

                    SMeshBuffer *buf = new SMeshBuffer();
                    buf->material_ = mb->GetMaterial();

                    buf->vertices_.Reallocate(vcount);
                    buf->indices_.Reallocate(icount);

                    core::Map<const video::S3DVertex, const u16> sind; // search index for fast operation
                    typedef core::Map<const video::S3DVertex, const u16>::Node snode;

                    // Main algorithm
                    u32 highest = 0;
                    u32 drawcalls = 0;
                    for (;;)
                    {
                        if (tc[highest].drawn)
                        {
                            bool found = false;
                            float hiscore = 0;
                            for (u32 t = 0; t < tcount; t++)
                            {
                                if (!tc[t].drawn)
                                {
                                    if (tc[t].score > hiscore)
                                    {
                                        highest = t;
                                        hiscore = tc[t].score;
                                        found = true;
                                    }
                                }
                            }
                            if (!found)
                                break;
                        }

                        // Output the best triangle
                        u16 newind = buf->vertices_.Size();

                        snode *s = sind.find(v[tc[highest].ind[0]]);

                        if (!s)
                        {
                            buf->vertices_.PushBack(v[tc[highest].ind[0]]);
                            buf->indices_.PushBack(newind);
                            sind.insert(v[tc[highest].ind[0]], newind);
                            newind++;
                        }
                        else
                        {
                            buf->indices_.PushBack(s->getValue());
                        }

                        s = sind.find(v[tc[highest].ind[1]]);

                        if (!s)
                        {
                            buf->vertices_.PushBack(v[tc[highest].ind[1]]);
                            buf->indices_.PushBack(newind);
                            sind.insert(v[tc[highest].ind[1]], newind);
                            newind++;
                        }
                        else
                        {
                            buf->indices_.PushBack(s->getValue());
                        }

                        s = sind.find(v[tc[highest].ind[2]]);

                        if (!s)
                        {
                            buf->vertices_.PushBack(v[tc[highest].ind[2]]);
                            buf->indices_.PushBack(newind);
                            sind.insert(v[tc[highest].ind[2]], newind);
                        }
                        else
                        {
                            buf->indices_.PushBack(s->getValue());
                        }

                        vc[tc[highest].ind[0]].NumActiveTris--;
                        vc[tc[highest].ind[1]].NumActiveTris--;
                        vc[tc[highest].ind[2]].NumActiveTris--;

                        tc[highest].drawn = true;

                        for (u16 j = 0; j < 3; j++)
                        {
                            vcache *vert = &vc[tc[highest].ind[j]];
                            for (u16 t = 0; t < vert->tris.Size(); t++)
                            {
                                if (highest == vert->tris[t])
                                {
                                    vert->tris.Erase(t);
                                    break;
                                }
                            }
                        }

                        lru.add(tc[highest].ind[0]);
                        lru.add(tc[highest].ind[1]);
                        highest = lru.add(tc[highest].ind[2], true);
                        drawcalls++;
                    }

                    buf->SetBoundingBox(mb->GetBoundingBox());
                    newmesh->AddMeshBuffer(buf);
                    //buf->drop();
                }
                break;
                case video::EVT_2TCOORDS:
                {
                    video::S3DVertex2TCoords *v = (video::S3DVertex2TCoords *) mb->GetVertices();

                    SMeshBufferLightMap *buf = new SMeshBufferLightMap();
                    buf->material_ = mb->GetMaterial();

                    buf->vertices_.Reallocate(vcount);
                    buf->indices_.Reallocate(icount);

                    core::Map<const video::S3DVertex2TCoords, const u16> sind; // search index for fast operation
                    typedef core::Map<const video::S3DVertex2TCoords, const u16>::Node snode;

                    // Main algorithm
                    u32 highest = 0;
                    u32 drawcalls = 0;
                    for (;;)
                    {
                        if (tc[highest].drawn)
                        {
                            bool found = false;
                            float hiscore = 0;
                            for (u32 t = 0; t < tcount; t++)
                            {
                                if (!tc[t].drawn)
                                {
                                    if (tc[t].score > hiscore)
                                    {
                                        highest = t;
                                        hiscore = tc[t].score;
                                        found = true;
                                    }
                                }
                            }
                            if (!found)
                                break;
                        }

                        // Output the best triangle
                        u16 newind = buf->vertices_.Size();

                        snode *s = sind.find(v[tc[highest].ind[0]]);

                        if (!s)
                        {
                            buf->vertices_.PushBack(v[tc[highest].ind[0]]);
                            buf->indices_.PushBack(newind);
                            sind.insert(v[tc[highest].ind[0]], newind);
                            newind++;
                        }
                        else
                        {
                            buf->indices_.PushBack(s->getValue());
                        }

                        s = sind.find(v[tc[highest].ind[1]]);

                        if (!s)
                        {
                            buf->vertices_.PushBack(v[tc[highest].ind[1]]);
                            buf->indices_.PushBack(newind);
                            sind.insert(v[tc[highest].ind[1]], newind);
                            newind++;
                        }
                        else
                        {
                            buf->indices_.PushBack(s->getValue());
                        }

                        s = sind.find(v[tc[highest].ind[2]]);

                        if (!s)
                        {
                            buf->vertices_.PushBack(v[tc[highest].ind[2]]);
                            buf->indices_.PushBack(newind);
                            sind.insert(v[tc[highest].ind[2]], newind);
                        }
                        else
                        {
                            buf->indices_.PushBack(s->getValue());
                        }

                        vc[tc[highest].ind[0]].NumActiveTris--;
                        vc[tc[highest].ind[1]].NumActiveTris--;
                        vc[tc[highest].ind[2]].NumActiveTris--;

                        tc[highest].drawn = true;

                        for (u16 j = 0; j < 3; j++)
                        {
                            vcache *vert = &vc[tc[highest].ind[j]];
                            for (u16 t = 0; t < vert->tris.Size(); t++)
                            {
                                if (highest == vert->tris[t])
                                {
                                    vert->tris.Erase(t);
                                    break;
                                }
                            }
                        }

                        lru.add(tc[highest].ind[0]);
                        lru.add(tc[highest].ind[1]);
                        highest = lru.add(tc[highest].ind[2]);
                        drawcalls++;
                    }

                    buf->SetBoundingBox(mb->GetBoundingBox());
                    newmesh->AddMeshBuffer(buf);
                    //buf->drop();

                }
                break;
                case video::EVT_TANGENTS:
                {
                    video::S3DVertexTangents *v = (video::S3DVertexTangents *) mb->GetVertices();

                    SMeshBufferTangents *buf = new SMeshBufferTangents();
                    buf->material_ = mb->GetMaterial();

                    buf->vertices_.Reallocate(vcount);
                    buf->indices_.Reallocate(icount);

                    core::Map<const video::S3DVertexTangents, const u16> sind; // search index for fast operation
                    typedef core::Map<const video::S3DVertexTangents, const u16>::Node snode;

                    // Main algorithm
                    u32 highest = 0;
                    u32 drawcalls = 0;
                    for (;;)
                    {
                        if (tc[highest].drawn)
                        {
                            bool found = false;
                            float hiscore = 0;
                            for (u32 t = 0; t < tcount; t++)
                            {
                                if (!tc[t].drawn)
                                {
                                    if (tc[t].score > hiscore)
                                    {
                                        highest = t;
                                        hiscore = tc[t].score;
                                        found = true;
                                    }
                                }
                            }
                            if (!found)
                                break;
                        }

                        // Output the best triangle
                        u16 newind = buf->vertices_.Size();

                        snode *s = sind.find(v[tc[highest].ind[0]]);

                        if (!s)
                        {
                            buf->vertices_.PushBack(v[tc[highest].ind[0]]);
                            buf->indices_.PushBack(newind);
                            sind.insert(v[tc[highest].ind[0]], newind);
                            newind++;
                        }
                        else
                        {
                            buf->indices_.PushBack(s->getValue());
                        }

                        s = sind.find(v[tc[highest].ind[1]]);

                        if (!s)
                        {
                            buf->vertices_.PushBack(v[tc[highest].ind[1]]);
                            buf->indices_.PushBack(newind);
                            sind.insert(v[tc[highest].ind[1]], newind);
                            newind++;
                        }
                        else
                        {
                            buf->indices_.PushBack(s->getValue());
                        }

                        s = sind.find(v[tc[highest].ind[2]]);

                        if (!s)
                        {
                            buf->vertices_.PushBack(v[tc[highest].ind[2]]);
                            buf->indices_.PushBack(newind);
                            sind.insert(v[tc[highest].ind[2]], newind);
                        }
                        else
                        {
                            buf->indices_.PushBack(s->getValue());
                        }

                        vc[tc[highest].ind[0]].NumActiveTris--;
                        vc[tc[highest].ind[1]].NumActiveTris--;
                        vc[tc[highest].ind[2]].NumActiveTris--;

                        tc[highest].drawn = true;

                        for (u16 j = 0; j < 3; j++)
                        {
                            vcache *vert = &vc[tc[highest].ind[j]];
                            for (u16 t = 0; t < vert->tris.Size(); t++)
                            {
                                if (highest == vert->tris[t])
                                {
                                    vert->tris.Erase(t);
                                    break;
                                }
                            }
                        }

                        lru.add(tc[highest].ind[0]);
                        lru.add(tc[highest].ind[1]);
                        highest = lru.add(tc[highest].ind[2]);
                        drawcalls++;
                    }

                    buf->SetBoundingBox(mb->GetBoundingBox());
                    newmesh->AddMeshBuffer(buf);
                    //buf->drop();
                }
                break;
                }

                delete[] vc;
                delete[] tc;

            } // for each meshbuffer

            return newmesh;
        }

    } // end namespace scene
} // end namespace irr