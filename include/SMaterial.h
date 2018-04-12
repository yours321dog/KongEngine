// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _SMATERIAL_H_
#define _SMATERIAL_H_
#include "SColor.h"
#include "SMaterialLayer.h"

namespace kong
{
    namespace video
    {

        //! Maximum number of texture an SMaterial can have.
        const u32 MATERIAL_MAX_TEXTURES = _KONG_MATERIAL_MAX_TEXTURES_;

        class SMaterial
        {
        public:
            bool operator!=(const SMaterial &other) const;
            bool operator==(const SMaterial &other) const;

            //! Gets the texture transformation matrix for level i
            /** \param i The desired level. Must not be larger than MATERIAL_MAX_TEXTURES.
            \return Texture matrix for texture level i. */
            core::Matrixf& GetTextureMatrix(u32 i)
            {
                return texture_layer_[i].GetTextureMatrix();
            }

            //! Gets the immutable texture transformation matrix for level i
            /** \param i The desired level.
            \return Texture matrix for texture level i, or identity matrix for levels larger than MATERIAL_MAX_TEXTURES. */
            const core::Matrixf& GetTextureMatrix(u32 i) const
            {
                if (i<MATERIAL_MAX_TEXTURES)
                    return texture_layer_[i].GetTextureMatrix();
                else
                    return core::identity_matrix;
            }

            //! Sets the i-th texture transformation matrix
            /** \param i The desired level.
            \param mat Texture matrix for texture level i. */
            void SetTextureMatrix(u32 i, const core::Matrixf& mat)
            {
                if (i >= MATERIAL_MAX_TEXTURES)
                    return;
                texture_layer_[i].SetTextureMatrix(mat);
            }

            //! Gets the i-th texture
            /** \param i The desired level.
            \return Texture for texture level i, if defined, else 0. */
            ITexture* GetTexture(u32 i) const
            {
                return i < MATERIAL_MAX_TEXTURES ? texture_layer_[i].Texture : 0;
            }

            //! Sets the i-th texture
            /** If i>=MATERIAL_MAX_TEXTURES this setting will be ignored.
            \param i The desired level.
            \param tex Texture for texture level i. */
            void SetTexture(u32 i, ITexture* tex)
            {
                if (i >= MATERIAL_MAX_TEXTURES)
                    return;
                texture_layer_[i].Texture = tex;
            }

            SColor ambient_color_;
            SColor diffuse_color_;
            SColor specular_color_;
            SColor emissive_color_;

            f32 shininess_;

            bool use_mip_maps_ : 1;

            //! Texture layer array.
            SMaterialLayer texture_layer_[MATERIAL_MAX_TEXTURES];
        };

        inline bool SMaterial::operator!=(const SMaterial& other) const
        {
            const bool different = ambient_color_ != other.ambient_color_ ||
                diffuse_color_ != other.diffuse_color_ ||
                specular_color_ != other.specular_color_ ||
                emissive_color_ != other.emissive_color_ ||
                shininess_ != other.shininess_ ||
                use_mip_maps_ != other.use_mip_maps_;
            return different;
        }

        inline bool SMaterial::operator==(const SMaterial& other) const
        {
            return !(*this != other);
        }

        //! global const identity Material
        KONG_API extern SMaterial IdentityMaterial;
    } // end namespace video
} // end namespace kong

#endif