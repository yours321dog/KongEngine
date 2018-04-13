// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _SMATERIAL_H_
#define _SMATERIAL_H_
#include "SColor.h"
#include "SMaterialLayer.h"
#include "EMaterialFlags.h"
#include "EMaterialTypes.h"

namespace kong
{
    namespace video
    {

        //! Maximum number of texture an SMaterial can have.
        const u32 MATERIAL_MAX_TEXTURES = _KONG_MATERIAL_MAX_TEXTURES_;

        //! Flag for EMT_ONETEXTURE_BLEND, ( BlendFactor ) BlendFunc = source * sourceFactor + dest * destFactor
        enum E_BLEND_FACTOR
        {
            EBF_ZERO = 0,			//!< src & dest	(0, 0, 0, 0)
            EBF_ONE,					//!< src & dest	(1, 1, 1, 1)
            EBF_DST_COLOR, 				//!< src	(destR, destG, destB, destA)
            EBF_ONE_MINUS_DST_COLOR,	//!< src	(1-destR, 1-destG, 1-destB, 1-destA)
            EBF_SRC_COLOR,				//!< dest	(srcR, srcG, srcB, srcA)
            EBF_ONE_MINUS_SRC_COLOR, 	//!< dest	(1-srcR, 1-srcG, 1-srcB, 1-srcA)
            EBF_SRC_ALPHA,				//!< src & dest	(srcA, srcA, srcA, srcA)
            EBF_ONE_MINUS_SRC_ALPHA,	//!< src & dest	(1-srcA, 1-srcA, 1-srcA, 1-srcA)
            EBF_DST_ALPHA,				//!< src & dest	(destA, destA, destA, destA)
            EBF_ONE_MINUS_DST_ALPHA,	//!< src & dest	(1-destA, 1-destA, 1-destA, 1-destA)
            EBF_SRC_ALPHA_SATURATE		//!< src	(min(srcA, 1-destA), idem, ...)
        };

        //! Values defining the blend operation used when blend is enabled
        enum E_BLEND_OPERATION
        {
            EBO_NONE = 0,	//!< No blending happens
            EBO_ADD,		//!< Default blending adds the color values
            EBO_SUBTRACT,	//!< This mode subtracts the color values
            EBO_REVSUBTRACT,//!< This modes subtracts destination from source
            EBO_MIN,		//!< Choose minimum value of each color channel
            EBO_MAX,		//!< Choose maximum value of each color channel
            EBO_MIN_FACTOR,	//!< Choose minimum value of each color channel after applying blend factors, not widely supported
            EBO_MAX_FACTOR,	//!< Choose maximum value of each color channel after applying blend factors, not widely supported
            EBO_MIN_ALPHA,	//!< Choose minimum value of each color channel based on alpha value, not widely supported
            EBO_MAX_ALPHA	//!< Choose maximum value of each color channel based on alpha value, not widely supported
        };

        //! MaterialTypeParam: e.g. DirectX: D3DTOP_MODULATE, D3DTOP_MODULATE2X, D3DTOP_MODULATE4X
        enum E_MODULATE_FUNC
        {
            EMFN_MODULATE_1X = 1,
            EMFN_MODULATE_2X = 2,
            EMFN_MODULATE_4X = 4
        };

        //! Comparison function, e.g. for depth buffer test
        enum E_COMPARISON_FUNC
        {
            //! Test never succeeds, this equals disable
            ECFN_NEVER = 0,
            //! <= test, default for e.g. depth test
            ECFN_LESSEQUAL = 1,
            //! Exact equality
            ECFN_EQUAL = 2,
            //! exclusive less comparison, i.e. <
            ECFN_LESS,
            //! Succeeds almost always, except for exact equality
            ECFN_NOTEQUAL,
            //! >= test
            ECFN_GREATEREQUAL,
            //! inverse of <=
            ECFN_GREATER,
            //! test succeeds always
            ECFN_ALWAYS
        };

        //! Enum values for enabling/disabling color planes for rendering
        enum E_COLOR_PLANE
        {
            //! No color enabled
            ECP_NONE = 0,
            //! Alpha enabled
            ECP_ALPHA = 1,
            //! Red enabled
            ECP_RED = 2,
            //! Green enabled
            ECP_GREEN = 4,
            //! Blue enabled
            ECP_BLUE = 8,
            //! All colors, no alpha
            ECP_RGB = 14,
            //! All planes enabled
            ECP_ALL = 15
        };

        //! These flags are used to specify the anti-aliasing and smoothing modes
        /** Techniques supported are multisampling, geometry smoothing, and alpha
        to coverage.
        Some drivers don't support a per-material setting of the anti-aliasing
        modes. In those cases, FSAA/multisampling is defined by the device mode
        chosen upon creation via irr::SIrrCreationParameters.
        */
        enum E_ANTI_ALIASING_MODE
        {
            //! Use to turn off anti-aliasing for this material
            EAAM_OFF = 0,
            //! Default anti-aliasing mode
            EAAM_SIMPLE = 1,
            //! High-quality anti-aliasing, not always supported, automatically enables SIMPLE mode
            EAAM_QUALITY = 3,
            //! Line smoothing
            EAAM_LINE_SMOOTH = 4,
            //! point smoothing, often in software and slow, only with OpenGL
            EAAM_POINT_SMOOTH = 8,
            //! All typical anti-alias and smooth modes
            EAAM_FULL_BASIC = 15,
            //! Enhanced anti-aliasing for transparent materials
            /** Usually used with EMT_TRANSPARENT_ALPHA_REF and multisampling. */
            EAAM_ALPHA_TO_COVERAGE = 16
        };

        //! These flags allow to define the interpretation of vertex color when lighting is enabled
        /** Without lighting being enabled the vertex color is the only value defining the fragment color.
        Once lighting is enabled, the four values for diffuse, ambient, emissive, and specular take over.
        With these flags it is possible to define which lighting factor shall be defined by the vertex color
        instead of the lighting factor which is the same for all faces of that material.
        The default is to use vertex color for the diffuse value, another pretty common value is to use
        vertex color for both diffuse and ambient factor. */
        enum E_COLOR_MATERIAL
        {
            //! Don't use vertex color for lighting
            ECM_NONE = 0,
            //! Use vertex color for diffuse light, this is default
            ECM_DIFFUSE,
            //! Use vertex color for ambient light
            ECM_AMBIENT,
            //! Use vertex color for emissive light
            ECM_EMISSIVE,
            //! Use vertex color for specular light
            ECM_SPECULAR,
            //! Use vertex color for both diffuse and ambient light
            ECM_DIFFUSE_AND_AMBIENT
        };

        //! Flags for the definition of the polygon offset feature
        /** These flags define whether the offset should be into the screen, or towards the eye. */
        enum E_POLYGON_OFFSET
        {
            //! Push pixel towards the far plane, away from the eye
            /** This is typically used for rendering inner areas. */
            EPO_BACK = 0,
            //! Pull pixels towards the camera.
            /** This is typically used for polygons which should appear on top
            of other elements, such as decals. */
            EPO_FRONT = 1
        };

        class SMaterial
        {
        public:
            SMaterial(): shininess_(0), material_type_param_(0), use_mip_maps_(false)
            {
            }

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

            //! Sets the Material flag to the given value
            /** \param flag The flag to be set.
            \param value The new value for the flag. */
            void SetFlag(E_MATERIAL_FLAG flag, bool value)
            {
                switch (flag)
                {
                case EMF_WIREFRAME:
                    Wireframe = value; break;
                case EMF_POINTCLOUD:
                    PointCloud = value; break;
                case EMF_GOURAUD_SHADING:
                    GouraudShading = value; break;
                case EMF_LIGHTING:
                    Lighting = value; break;
                case EMF_ZBUFFER:
                    ZBuffer = value; break;
                case EMF_ZWRITE_ENABLE:
                    ZWriteEnable = value; break;
                case EMF_BACK_FACE_CULLING:
                    BackfaceCulling = value; break;
                case EMF_FRONT_FACE_CULLING:
                    FrontfaceCulling = value; break;
                case EMF_BILINEAR_FILTER:
                {
                    for (u32 i = 0; i<MATERIAL_MAX_TEXTURES; ++i)
                        texture_layer_[i].BilinearFilter = value;
                }
                break;
                case EMF_TRILINEAR_FILTER:
                {
                    for (u32 i = 0; i<MATERIAL_MAX_TEXTURES; ++i)
                        texture_layer_[i].TrilinearFilter = value;
                }
                break;
                case EMF_ANISOTROPIC_FILTER:
                {
                    if (value)
                        for (u32 i = 0; i<MATERIAL_MAX_TEXTURES; ++i)
                            texture_layer_[i].AnisotropicFilter = 0xFF;
                    else
                        for (u32 i = 0; i<MATERIAL_MAX_TEXTURES; ++i)
                            texture_layer_[i].AnisotropicFilter = 0;
                }
                break;
                case EMF_FOG_ENABLE:
                    FogEnable = value; break;
                case EMF_NORMALIZE_NORMALS:
                    NormalizeNormals = value; break;
                case EMF_TEXTURE_WRAP:
                {
                    for (u32 i = 0; i<MATERIAL_MAX_TEXTURES; ++i)
                    {
                        texture_layer_[i].TextureWrapU = (E_TEXTURE_CLAMP)value;
                        texture_layer_[i].TextureWrapV = (E_TEXTURE_CLAMP)value;
                    }
                }
                break;
                case EMF_ANTI_ALIASING:
                    AntiAliasing = value ? EAAM_SIMPLE : EAAM_OFF; break;
                case EMF_COLOR_MASK:
                    ColorMask = value ? ECP_ALL : ECP_NONE; break;
                case EMF_COLOR_MATERIAL:
                    ColorMaterial = value ? ECM_DIFFUSE : ECM_NONE; break;
                case EMF_USE_MIP_MAPS:
                    use_mip_maps_ = value; break;
                case EMF_BLEND_OPERATION:
                    BlendOperation = value ? EBO_ADD : EBO_NONE; break;
                case EMF_POLYGON_OFFSET:
                    PolygonOffsetFactor = value ? 1 : 0;
                    PolygonOffsetDirection = EPO_BACK;
                    break;
                default:
                    break;
                }
            }

            //! Gets the Material flag
            /** \param flag The flag to query.
            \return The current value of the flag. */
            bool GetFlag(E_MATERIAL_FLAG flag) const
            {
                switch (flag)
                {
                case EMF_WIREFRAME:
                    return Wireframe;
                case EMF_POINTCLOUD:
                    return PointCloud;
                case EMF_GOURAUD_SHADING:
                    return GouraudShading;
                case EMF_LIGHTING:
                    return Lighting;
                case EMF_ZBUFFER:
                    return ZBuffer != ECFN_NEVER;
                case EMF_ZWRITE_ENABLE:
                    return ZWriteEnable;
                case EMF_BACK_FACE_CULLING:
                    return BackfaceCulling;
                case EMF_FRONT_FACE_CULLING:
                    return FrontfaceCulling;
                case EMF_BILINEAR_FILTER:
                    return texture_layer_[0].BilinearFilter;
                case EMF_TRILINEAR_FILTER:
                    return texture_layer_[0].TrilinearFilter;
                case EMF_ANISOTROPIC_FILTER:
                    return texture_layer_[0].AnisotropicFilter != 0;
                case EMF_FOG_ENABLE:
                    return FogEnable;
                case EMF_NORMALIZE_NORMALS:
                    return NormalizeNormals;
                case EMF_TEXTURE_WRAP:
                    return !(texture_layer_[0].TextureWrapU ||
                        texture_layer_[0].TextureWrapV ||
                        texture_layer_[1].TextureWrapU ||
                        texture_layer_[1].TextureWrapV ||
                        texture_layer_[2].TextureWrapU ||
                        texture_layer_[2].TextureWrapV ||
                        texture_layer_[3].TextureWrapU ||
                        texture_layer_[3].TextureWrapV);
                case EMF_ANTI_ALIASING:
                    return (AntiAliasing == 1);
                case EMF_COLOR_MASK:
                    return (ColorMask != ECP_NONE);
                case EMF_COLOR_MATERIAL:
                    return (ColorMaterial != ECM_NONE);
                case EMF_USE_MIP_MAPS:
                    return use_mip_maps_;
                case EMF_BLEND_OPERATION:
                    return BlendOperation != EBO_NONE;
                case EMF_POLYGON_OFFSET:
                    return PolygonOffsetFactor != 0;
                }

                return false;
            }

            SColor ambient_color_;
            SColor diffuse_color_;
            SColor specular_color_;
            SColor emissive_color_;

            f32 shininess_;

            //! Type of the material. Specifies how everything is blended together
            E_MATERIAL_TYPE MaterialType;

            //! Thickness of non-3dimensional elements such as lines and points.
            f32 Thickness;

            //! Is the ZBuffer enabled? Default: ECFN_LESSEQUAL
            /** Values are from E_COMPARISON_FUNC. */
            u8 ZBuffer;

            //! Sets the antialiasing mode
            /** Values are chosen from E_ANTI_ALIASING_MODE. Default is
            EAAM_SIMPLE|EAAM_LINE_SMOOTH, i.e. simple multi-sample
            anti-aliasing and lime smoothing is enabled. */
            u8 AntiAliasing;

            //! Defines the enabled color planes
            /** Values are defined as or'ed values of the E_COLOR_PLANE enum.
            Only enabled color planes will be rendered to the current render
            target. Typical use is to disable all colors when rendering only to
            depth or stencil buffer, or using Red and Green for Stereo rendering. */
            u8 ColorMask : 4;

            //! Defines the interpretation of vertex color in the lighting equation
            /** Values should be chosen from E_COLOR_MATERIAL.
            When lighting is enabled, vertex color can be used instead of the
            material values for light modulation. This allows to easily change e.g. the
            diffuse light behavior of each face. The default, ECM_DIFFUSE, will result in
            a very similar rendering as with lighting turned off, just with light shading. */
            u8 ColorMaterial : 3;

            //! Store the blend operation of choice
            /** Values to be chosen from E_BLEND_OPERATION. The actual way to use this value
            is not yet determined, so ignore it for now. */
            E_BLEND_OPERATION BlendOperation : 4;

            //! Factor specifying how far the polygon offset should be made
            /** Specifying 0 disables the polygon offset. The direction is specified spearately.
            The factor can be from 0 to 7.*/
            u8 PolygonOffsetFactor : 3;

            //! Flag defining the direction the polygon offset is applied to.
            /** Can be to front or to back, specififed by values from E_POLYGON_OFFSET. */
            E_POLYGON_OFFSET PolygonOffsetDirection : 1;

            //! Draw as wireframe or filled triangles? Default: false
            /** The user can access a material flag using
            \code material.Wireframe=true \endcode
            or \code material.setFlag(EMF_WIREFRAME, true); \endcode */
            bool Wireframe : 1;

            //! Draw as point cloud or filled triangles? Default: false
            bool PointCloud : 1;

            //! Flat or Gouraud shading? Default: true
            bool GouraudShading : 1;

            //! Will this material be lighted? Default: true
            bool Lighting : 1;

            //! Is the zbuffer writeable or is it read-only. Default: true.
            /** This flag is forced to false if the MaterialType is a
            transparent type and the scene parameter
            ALLOW_ZWRITE_ON_TRANSPARENT is not set. */
            bool ZWriteEnable : 1;

            //! Is backface culling enabled? Default: true
            bool BackfaceCulling : 1;

            //! Is frontface culling enabled? Default: false
            bool FrontfaceCulling : 1;

            //! Is fog enabled? Default: false
            bool FogEnable : 1;

            //! Should normals be normalized?
            /** Always use this if the mesh lit and scaled. Default: false */
            bool NormalizeNormals : 1;

            //! Free parameter, dependent on the material type.
            /** Mostly ignored, used for example in EMT_PARALLAX_MAP_SOLID
            and EMT_TRANSPARENT_ALPHA_CHANNEL. */
            f32 material_type_param_;

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
                use_mip_maps_ != other.use_mip_maps_ ||
                material_type_param_ != other.material_type_param_ ||
                MaterialType != other.MaterialType ||
                ZBuffer != other.ZBuffer ||
                Thickness != other.Thickness ||
                AntiAliasing != other.AntiAliasing ||
                ColorMask != other.ColorMask ||
                ColorMaterial != other.ColorMask || 
                BlendOperation != other.BlendOperation ||
                PolygonOffsetDirection != other.PolygonOffsetDirection ||
                PolygonOffsetFactor != other.PolygonOffsetFactor ||
                Wireframe != other.Wireframe ||
                PointCloud != other.PointCloud ||
                Lighting != other.Lighting ||
                GouraudShading != other.GouraudShading ||
                ZWriteEnable != other.ZWriteEnable ||
                BackfaceCulling != other.BackfaceCulling ||
                FrontfaceCulling != other.FrontfaceCulling ||
                FogEnable != other.FogEnable ||
                NormalizeNormals != other.NormalizeNormals;
            
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