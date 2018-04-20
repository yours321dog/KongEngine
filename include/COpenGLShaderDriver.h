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

        class COpenGLShaderDriver : public COpenGLDriver
        {
        public:
            COpenGLShaderDriver(const SKongCreationParameters &params, io::IFileSystem* file_system, CKongDeviceWin32* device,
                io::SPath vertex_path = "./Shaders/base.vs", io::SPath fragment_path = "./Shaders/base.fs");

            bool InitDriver(CKongDeviceWin32 *device) override;

            //! Sets transformation matrices.
            void SetTransform(u32 state, const core::Matrixf& mat) override;

            //! Draws a mesh buffer
            void DrawMeshBuffer(const scene::IMeshBuffer* mesh_buffer) override;

            //! sets the current Texture
            //! Returns whether setting was a success or not.
            bool SetActiveTexture(u32 stage, const video::ITexture* texture) override;

        protected:


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