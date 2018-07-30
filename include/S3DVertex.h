// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _S3DVERTEX_H_
#define _S3DVERTEX_H_
#include "Vector.h"
#include "SColor.h"


namespace kong
{
    namespace video
    {
        //! Enumeration for all vertex types there are.
        enum E_VERTEX_TYPE
        {
            //! Standard vertex type used by the Irrlicht engine, video::S3DVertex.
            EVT_STANDARD = 0,

            //! Vertex with two texture coordinates, video::S3DVertex2TCoords.
            /** Usually used for geometry with lightmaps or other special materials. */
            EVT_2TCOORDS,

            //! Vertex with a tangent and binormal vector, video::S3DVertexTangents.
            /** Usually used for tangent space normal mapping. */
            EVT_TANGENTS
        };

        enum E_INDEX_TYPE
        {
            EIT_16BIT = 0,
            EIT_32BIT
        };

        //! Array holding the built in vertex type names
        const char* const sBuiltInVertexTypeNames[] =
        {
            "standard",
            "2tcoords",
            "tangents",
            0
        };

        class S3DVertex
        {
        public:
            S3DVertex();

            S3DVertex(f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz, SColor c, f32 tu, f32 tv);

            S3DVertex(const core::Vector3Df &pos, const core::Vector3Df & normal, 
                const SColor &color, const core::Vector2Df &texcoord);

            S3DVertex(const core::Vector3Df &pos, const core::Vector3Df & normal,
                const SColor &color, const core::Vector2Df &texcoord, const core::Vector3Df &barycentric);

            bool operator==(const S3DVertex &other) const;

            bool operator!=(const S3DVertex& other) const;

            bool operator<(const S3DVertex& other) const;

            S3DVertex GetInterpolated(const S3DVertex &other, f32 t) const;

            E_VERTEX_TYPE GetType() const;

            core::Vector3Df pos_;
            core::Vector3Df normal_;
            core::Vector2Df texcoord_;
            core::Vector3Df barycentric_;
            SColor color_;
        };

        inline S3DVertex::S3DVertex()
            = default;

        inline S3DVertex::S3DVertex(f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz, SColor c, f32 tu, f32 tv)
            : pos_(x, y, z), normal_(nx, ny, nz), texcoord_(tu, tv), color_(c)
        {
        }

        inline S3DVertex::S3DVertex(const core::Vector3Df& pos, const core::Vector3Df& normal, const SColor& color,
            const core::Vector2Df& texcoord)
            : pos_(pos), normal_(normal), texcoord_(texcoord), color_(color)
        {

        }

        inline S3DVertex::S3DVertex(const core::Vector3Df& pos, const core::Vector3Df& normal, const SColor& color,
            const core::Vector2Df& texcoord, const core::Vector3Df& barycentric)
            : pos_(pos), normal_(normal), texcoord_(texcoord), barycentric_(barycentric), color_(color)
        {

        }

        inline bool S3DVertex::operator==(const S3DVertex& other) const
        {
            return pos_ == other.pos_ && normal_ == other.normal_ && texcoord_ == other.texcoord_ && color_ == other.color_;
        }

        inline bool S3DVertex::operator!=(const S3DVertex& other) const
        {
            return pos_ != other.pos_ || normal_ != other.normal_ || texcoord_ != other.texcoord_ || color_ != other.color_;
        }

        inline bool S3DVertex::operator<(const S3DVertex& other) const
        {
            return ((pos_ < other.pos_) ||
                ((pos_ == other.pos_) && (normal_ < other.normal_)) ||
                ((pos_ == other.pos_) && (normal_ == other.normal_) && (color_ < other.color_)) ||
                ((pos_ == other.pos_) && (normal_ == other.normal_) && (color_ == other.color_) && (texcoord_ < other.texcoord_)));
        }

        inline S3DVertex S3DVertex::GetInterpolated(const S3DVertex& other, f32 t) const
        {
            t = core::clamp(t, 0.f, 1.f);
            S3DVertex tmp;
            tmp.pos_.Interpolation(pos_, other.pos_, t);
            tmp.normal_.Interpolation(normal_, other.normal_, t);
            tmp.texcoord_.Interpolation(texcoord_, other.texcoord_, t);
            tmp.color_.Interpolation(color_, other.color_, t);
            return tmp;
        }

        inline E_VERTEX_TYPE S3DVertex::GetType() const
        {
            return EVT_STANDARD;
        }

        class S3DVertex2TCoords : public S3DVertex
        {
        public:
            //! default constructor
            S3DVertex2TCoords() : S3DVertex() {}

            //! constructor with two different texture coords, but no normal
            S3DVertex2TCoords(f32 x, f32 y, f32 z, SColor c, f32 tu, f32 tv, f32 tu2, f32 tv2)
                : S3DVertex(x, y, z, 0.0f, 0.0f, 0.0f, c, tu, tv), texcoord2_(tu2, tv2) {}

            //! constructor with two different texture coords, but no normal
            S3DVertex2TCoords(const core::vector3df& pos, SColor color,
                const core::Vector<f32>& tcoords, const core::Vector<f32>& tcoords2)
                : S3DVertex(pos, core::vector3df(), color, tcoords), texcoord2_(tcoords2) {}

            //! constructor with all values
            S3DVertex2TCoords(const core::vector3df& pos, const core::vector3df& normal, const SColor& color,
                const core::Vector<f32>& tcoords, const core::Vector<f32>& tcoords2)
                : S3DVertex(pos, normal, color, tcoords), texcoord2_(tcoords2) {}

            //! constructor with all values
            S3DVertex2TCoords(f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz, SColor c, f32 tu, f32 tv, f32 tu2, f32 tv2)
                : S3DVertex(x, y, z, nx, ny, nz, c, tu, tv), texcoord2_(tu2, tv2) {}

            //! constructor with the same texture coords and normal
            S3DVertex2TCoords(f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz, SColor c, f32 tu, f32 tv)
                : S3DVertex(x, y, z, nx, ny, nz, c, tu, tv), texcoord2_(tu, tv) {}

            //! constructor with the same texture coords and normal
            S3DVertex2TCoords(const core::vector3df& pos, const core::vector3df& normal,
                SColor color, const core::Vector<f32>& tcoords)
                : S3DVertex(pos, normal, color, tcoords), texcoord2_(tcoords) {}

            //! constructor from S3DVertex
            S3DVertex2TCoords(S3DVertex& o) : S3DVertex(o) {}

            //! Second set of texture coordinates
            core::Vector<f32> texcoord2_;

            //! Equality operator
            bool operator==(const S3DVertex2TCoords& other) const
            {
                return ((static_cast<S3DVertex>(*this) == other) &&
                    (texcoord2_ == other.texcoord2_));
            }

            //! Inequality operator
            bool operator!=(const S3DVertex2TCoords& other) const
            {
                return ((static_cast<S3DVertex>(*this) != other) ||
                    (texcoord2_ != other.texcoord2_));
            }

            bool operator<(const S3DVertex2TCoords& other) const
            {
                return ((static_cast<S3DVertex>(*this) < other) ||
                    ((static_cast<S3DVertex>(*this) == other) && (texcoord2_ < other.texcoord2_)));
            }

            E_VERTEX_TYPE GetType() const
            {
                return EVT_2TCOORDS;
            }

            S3DVertex2TCoords GetInterpolated(const S3DVertex2TCoords& other, f32 d)
            {
                d = core::clamp(d, 0.0f, 1.0f);
                return S3DVertex2TCoords(pos_.GetInterpolation(other.pos_, d),
                    normal_.GetInterpolation(other.normal_, d),
                    color_.GetInterpolated(other.color_, d),
                    texcoord_.GetInterpolation(other.texcoord_, d),
                    texcoord2_.GetInterpolation(other.texcoord2_, d));
            }
        };

        //! Vertex with a tangent and binormal vector.
        /** Usually used for tangent space normal mapping. */
        class S3DVertexTangents : public S3DVertex
        {
        public:
            //! default constructor
            S3DVertexTangents() : S3DVertex() { }

            //! constructor
            S3DVertexTangents(f32 x, f32 y, f32 z, f32 nx = 0.0f, f32 ny = 0.0f, f32 nz = 0.0f,
                SColor c = 0xFFFFFFFF, f32 tu = 0.0f, f32 tv = 0.0f,
                f32 tanx = 0.0f, f32 tany = 0.0f, f32 tanz = 0.0f,
                f32 bx = 0.0f, f32 by = 0.0f, f32 bz = 0.0f)
                : S3DVertex(x, y, z, nx, ny, nz, c, tu, tv), tangent_(tanx, tany, tanz), binormal_(bx, by, bz) { }

            //! constructor
            S3DVertexTangents(const core::vector3df& pos, SColor c,
                const core::vector2df& tcoords)
                : S3DVertex(pos, core::vector3df(), c, tcoords) { }

            //! constructor
            S3DVertexTangents(const core::vector3df& pos,
                const core::vector3df& normal, SColor c,
                const core::vector2df& tcoords,
                const core::vector3df& tangent = core::vector3df(),
                const core::vector3df& binormal = core::vector3df())
                : S3DVertex(pos, normal, c, tcoords), tangent_(tangent), binormal_(binormal) { }

            //! Tangent vector along the x-axis of the texture
            core::vector3df tangent_;

            //! Binormal vector (tangent x normal)
            core::vector3df binormal_;

            bool operator==(const S3DVertexTangents& other) const
            {
                return ((static_cast<S3DVertex>(*this) == other) &&
                    (tangent_ == other.tangent_) &&
                    (binormal_ == other.binormal_));
            }

            bool operator!=(const S3DVertexTangents& other) const
            {
                return ((static_cast<S3DVertex>(*this) != other) ||
                    (tangent_ != other.tangent_) ||
                    (binormal_ != other.binormal_));
            }

            bool operator<(const S3DVertexTangents& other) const
            {
                return ((static_cast<S3DVertex>(*this) < other) ||
                    ((static_cast<S3DVertex>(*this) == other) && (tangent_ < other.tangent_)) ||
                    ((static_cast<S3DVertex>(*this) == other) && (tangent_ == other.tangent_) && (binormal_ < other.binormal_)));
            }

            E_VERTEX_TYPE GetType() const
            {
                return EVT_TANGENTS;
            }

            S3DVertexTangents GetInterpolated(const S3DVertexTangents& other, f32 d)
            {
                d = core::clamp(d, 0.0f, 1.0f);
                return S3DVertexTangents(pos_.GetInterpolation(other.pos_, d),
                    normal_.GetInterpolation(other.normal_, d),
                    color_.GetInterpolated(other.color_, d),
                    texcoord_.GetInterpolation(other.texcoord_, d),
                    tangent_.GetInterpolation(other.tangent_, d),
                    binormal_.GetInterpolation(other.binormal_, d));
            }
        };
    }
}
#endif