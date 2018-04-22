// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _COPENGLSHADERDRIVER_H_
#define _COPENGLSHADERDRIVER_H_
#include "COpenGLDriver.h"
#include "IFileSystem.h"
#include "IShaderHelper.h"

namespace kong
{
    namespace video
    {
        enum SHADER_UNIFORM_TYPE
        {
            SL_TEXTURE0 = 0,
            SL_TEXTURE1,
            SL_TEXTURE2,
            SL_TEXTURE3,
            SL_LIGHT0,
            SL_LIGHT1,
            SL_LIGHT2,
            SL_LIGHT3,
            SL_COUNT
        };

        enum MATERIAL_VAL_TYPE
        {
            SL_MAT_AMBIENT,
            SL_MAT_DIFFUSE,
            SL_MAT_SPECULAR,
            SL_MAT_EMISSIVE,
            SL_MAT_SHININESS,
            SL_MATERIAL_COUNT
        };

        enum LIGHT_VAL_TYPE
        {
            SL_LIGHT_POSITION,
            SL_LIGHT_DIRECTION,
            SL_LIGHT_AMBIENT,
            SL_LIGHT_DIFFUSE,
            SL_LIGHT_SPECULAR,
            SL_LIGHT_ATTENUATION,
            SL_LIGHT_EXPONENT,
            SL_LIGHT_CUTOFF,
            SL_LIGHT_COUNT
        };

        const c8 *const shader_uniform_name[] = 
        {
            "texture0",
            "texutre1",
            "texutre2",
            "texutre3",
            "light0",
            "light1",
            "light2",
            "light3",
            nullptr
        };

        const c8 *const material_uniform_name[] =
        {
            "material.ambient",
            "material.diffuse",
            "material.specular",
            "material.emissive",
            "material.shininess",
            nullptr
        };

        const c8 *const light_uniform_name[] =
        {
            "position",
            "direction",
            "ambient",
            "diffuse",
            "specular",
            "attenuation",
            "exponent",
            "cutoff",
            nullptr
        };

        class COpenGLShaderDriver : public COpenGLDriver
        {
        public:
            COpenGLShaderDriver(const SKongCreationParameters &params, io::IFileSystem* file_system, CKongDeviceWin32* device,
                io::SPath vertex_path = "./Shaders/base.vs", io::SPath fragment_path = "./Shaders/base.fs");

            bool InitDriver(CKongDeviceWin32 *device) override;

            //! Sets a material.
            void SetMaterial(const SMaterial& material) override;

            //! Sets transformation matrices.
            void SetTransform(u32 state, const core::Matrixf& mat) override;

            //! Draws a mesh buffer
            void DrawMeshBuffer(const scene::IMeshBuffer* mesh_buffer) override;

            //! sets the current Texture
            //! Returns whether setting was a success or not.
            bool SetActiveTexture(u32 stage, const video::ITexture* texture) override;

            //! Deletes all dynamic lights which were previously added with addDynamicLight().
            void DeleteAllDynamicLights() override;

            //! adds a dynamic light, returning an index to the light
            s32 AddDynamicLight(const SLight& light) override;

            //! Set active camera position
            void SetActiveCameraPosition(core::Vector3Df position) const override;

        protected:
            void UpdateMaxSupportLights() override;

            void SetMaterialUniform(s32 material_val_type, const void *val) const;
            void SetMaterialUniform(s32 material_val_type, f32 val) const;
            void SetLightUniform(s32 light_idx, s32 light_val_type, const void *val) const;
            void SetLightUniform(s32 light_idx, s32 light_val_type, f32 val) const;
            void Enable(s32 idx) const;
            void Disable(s32 idx) const;
            const c8 *GetUniformName(s32 idx) const;

            IShaderHelper *shader_helper_;

            u32 vao_;
            u32 vbo_;
            u32 ebo_;

            io::SPath vertex_path_;
            io::SPath fragment_path_;
        };
    } // end namespace video
} // end namespace kong

#endif