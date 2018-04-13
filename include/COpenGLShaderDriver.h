// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _COPENGLSHADERDRIVER_H_
#define _COPENGLSHADERDRIVER_H_
#include "COpenGLDriver.h"
#include "IFileSystem.h"
#include "COpenGLShaderHelper.h"

namespace kong
{
    namespace video
    {
        class COpenGLShaderDriver : public COpenGLDriver
        {
        public:
            COpenGLShaderDriver(const SKongCreationParameters &params, io::IFileSystem* file_system, CKongDeviceWin32* device,
                io::SPath vertex_path = "./Shaders/base.vs", io::SPath fragment_path = "./Shaders/base.fs");

            bool InitDriver(CKongDeviceWin32 *device) override;

            //! Sets transformation matrices.
            void SetTransform(E_TRANSFORMATION_STATE state, const core::Matrixf& mat) override;

            //! Draws a mesh buffer
            void DrawMeshBuffer(const scene::IMeshBuffer* mesh_buffer) override;

            //! sets the current Texture
            //! Returns whether setting was a success or not.
            bool SetActiveTexture(u32 stage, const video::ITexture* texture) override;

        protected:
            COpenGLShaderHelper *shader_helper_;

            u32 vao_;
            u32 vbo_;
            u32 ebo_;

            io::SPath vertex_path_;
            io::SPath fragment_path_;
        };
    } // end namespace video
} // end namespace kong

#endif