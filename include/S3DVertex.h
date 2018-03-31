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
        class S3DVertex
        {
        public:
            S3DVertex();

            S3DVertex(f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz, SColor c, f32 tu, f32 tv);

            S3DVertex(const core::Vector3Df &pos, const core::Vector3Df & normal, 
                const SColor &color, const core::Vector2Df &texcoord);

            bool operator==(const S3DVertex &other) const;

            bool operator!=(const S3DVertex& other) const;

            S3DVertex GetInterpolated(const S3DVertex &other, f32 t) const;

            core::Vector3Df pos_;
            core::Vector3Df normal_;
            core::Vector2Df texcoord_;
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

        inline bool S3DVertex::operator==(const S3DVertex& other) const
        {
            return pos_ == other.pos_ && normal_ == other.normal_ && texcoord_ == other.texcoord_ && color_ == other.color_;
        }

        inline bool S3DVertex::operator!=(const S3DVertex& other) const
        {
            return pos_ != other.pos_ || normal_ != other.normal_ || texcoord_ != other.texcoord_ || color_ != other.color_;
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
    }
}
#endif