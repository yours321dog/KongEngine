// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef _SMATERIALLAYER_H_
#define _SMATERIALLAYER_H_

#include "Matrix.h"
#include "Allocator.h"

namespace kong
{
    namespace video
    {
        class ITexture;

        //! Texture coord clamp mode outside [0.0, 1.0]
        enum E_TEXTURE_CLAMP
        {
            //! Texture repeats
            ETC_REPEAT = 0,
            //! Texture is clamped to the last pixel
            ETC_CLAMP,
            //! Texture is clamped to the edge pixel
            ETC_CLAMP_TO_EDGE,
            //! Texture is clamped to the border pixel (if exists)
            ETC_CLAMP_TO_BORDER,
            //! Texture is alternatingly mirrored (0..1..0..1..0..)
            ETC_MIRROR,
            //! Texture is mirrored once and then clamped (0..1..0)
            ETC_MIRROR_CLAMP,
            //! Texture is mirrored once and then clamped to edge
            ETC_MIRROR_CLAMP_TO_EDGE,
            //! Texture is mirrored once and then clamped to border
            ETC_MIRROR_CLAMP_TO_BORDER
        };
        static const char* const aTextureClampNames[] = {
            "texture_clamp_repeat",
            "texture_clamp_clamp",
            "texture_clamp_clamp_to_edge",
            "texture_clamp_clamp_to_border",
            "texture_clamp_mirror",
            "texture_clamp_mirror_clamp",
            "texture_clamp_mirror_clamp_to_edge",
            "texture_clamp_mirror_clamp_to_border", 0 };

        //! Struct for holding material parameters which exist per texture layer
        class SMaterialLayer
        {
        public:
            //! Default constructor
            SMaterialLayer()
                : Texture(nullptr),
                TextureWrapU(ETC_REPEAT),
                TextureWrapV(ETC_REPEAT),
                BilinearFilter(true),
                TrilinearFilter(false),
                AnisotropicFilter(0),
                LODBias(0),
                texture_matrix_(0)
            {}

            //! Copy constructor
            /** \param other Material layer to copy from. */
            SMaterialLayer(const SMaterialLayer& other)
            {
                // This pointer is checked during assignment
                texture_matrix_ = nullptr;
                *this = other;
            }

            //! Destructor
            ~SMaterialLayer()
            {
                matrix_allocator_.destruct(texture_matrix_);
                matrix_allocator_.deallocate(texture_matrix_);
            }

            //! Assignment operator
            /** \param other Material layer to copy from.
            \return This material layer, updated. */
            SMaterialLayer& operator=(const SMaterialLayer& other)
            {
                // Check for self-assignment!
                if (this == &other)
                    return *this;

                Texture = other.Texture;
                if (texture_matrix_)
                {
                    if (other.texture_matrix_)
                        *texture_matrix_ = *other.texture_matrix_;
                    else
                    {
                        matrix_allocator_.destruct(texture_matrix_);
                        matrix_allocator_.deallocate(texture_matrix_);
                        texture_matrix_ = 0;
                    }
                }
                else
                {
                    if (other.texture_matrix_)
                    {
                        texture_matrix_ = matrix_allocator_.allocate(1);
                        matrix_allocator_.construct(texture_matrix_, *other.texture_matrix_);
                    }
                    else
                        texture_matrix_ = 0;
                }
                TextureWrapU = other.TextureWrapU;
                TextureWrapV = other.TextureWrapV;
                BilinearFilter = other.BilinearFilter;
                TrilinearFilter = other.TrilinearFilter;
                AnisotropicFilter = other.AnisotropicFilter;
                LODBias = other.LODBias;

                return *this;
            }

            //! Gets the texture transformation matrix
            /** \return Texture matrix of this layer. */
            core::Matrixf& GetTextureMatrix()
            {
                if (!texture_matrix_)
                {
                    texture_matrix_ = matrix_allocator_.allocate(1);
                    matrix_allocator_.construct(texture_matrix_, core::identity_matrix);
                }
                return *texture_matrix_;
            }

            //! Gets the immutable texture transformation matrix
            /** \return Texture matrix of this layer. */
            const core::Matrixf& GetTextureMatrix() const
            {
                if (texture_matrix_)
                    return *texture_matrix_;
                else
                    return core::identity_matrix;
            }

            //! Sets the texture transformation matrix to mat
            /** \param mat New texture matrix for this layer. */
            void SetTextureMatrix(const core::Matrixf& mat)
            {
                if (!texture_matrix_)
                {
                    texture_matrix_ = matrix_allocator_.allocate(1);
                    matrix_allocator_.construct(texture_matrix_, mat);
                }
                else
                    *texture_matrix_ = mat;
            }

            //! Inequality operator
            /** \param b Layer to compare to.
            \return True if layers are different, else false. */
            inline bool operator!=(const SMaterialLayer& b) const
            {
                bool different =
                    Texture != b.Texture ||
                    TextureWrapU != b.TextureWrapU ||
                    TextureWrapV != b.TextureWrapV ||
                    BilinearFilter != b.BilinearFilter ||
                    TrilinearFilter != b.TrilinearFilter ||
                    AnisotropicFilter != b.AnisotropicFilter ||
                    LODBias != b.LODBias;
                if (different)
                    return true;
                else
                    different |= (texture_matrix_ != b.texture_matrix_) &&
                    texture_matrix_ && b.texture_matrix_ &&
                    (*texture_matrix_ != *(b.texture_matrix_));
                return different;
            }

            //! Equality operator
            /** \param b Layer to compare to.
            \return True if layers are equal, else false. */
            inline bool operator==(const SMaterialLayer& b) const
            {
                return !(b != *this);
            }

            //! Texture
            ITexture* Texture;

            //! Texture Clamp Mode
            /** Values are taken from E_TEXTURE_CLAMP. */
            u8 TextureWrapU : 4;
            u8 TextureWrapV : 4;

            //! Is bilinear filtering enabled? Default: true
            bool BilinearFilter : 1;

            //! Is trilinear filtering enabled? Default: false
            /** If the trilinear filter flag is enabled,
            the bilinear filtering flag is ignored. */
            bool TrilinearFilter : 1;

            //! Is anisotropic filtering enabled? Default: 0, disabled
            /** In Irrlicht you can use anisotropic texture filtering
            in conjunction with bilinear or trilinear texture
            filtering to improve rendering results. Primitives
            will look less blurry with this flag switched on. The number gives
            the maximal anisotropy degree, and is often in the range 2-16.
            Value 1 is equivalent to 0, but should be avoided. */
            u8 AnisotropicFilter;

            //! Bias for the mipmap choosing decision.
            /** This value can make the textures more or less blurry than with the
            default value of 0. The value (divided by 8.f) is added to the mipmap level
            chosen initially, and thus takes a smaller mipmap for a region
            if the value is positive. */
            s8 LODBias;

        private:
            friend class SMaterial;
            kong::core::Allocator<core::Matrixf> matrix_allocator_;

            //! Texture Matrix
            /** Do not access this element directly as the internal
            ressource management has to cope with Null pointers etc. */
            core::Matrixf* texture_matrix_;
        };

    } // end namespace video
} // end namespace irr

#endif // __S_MATERIAL_LAYER_H_INCLUDED__
