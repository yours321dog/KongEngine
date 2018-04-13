// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "COpenGLShaderDriver.h"
#include "GL/glew.h"
#include "S3DVertex.h"
#include "IMeshBuffer.h"
#include "COpenGLTexture.h"

namespace kong
{
    namespace video
    {

        COpenGLShaderDriver::COpenGLShaderDriver(const SKongCreationParameters& params, io::IFileSystem* file_system, CKongDeviceWin32* device,
            io::SPath vertex_path, io::SPath fragment_path)
            : COpenGLDriver(params, file_system, device), shader_helper_(nullptr), vertex_path_(vertex_path), fragment_path_(fragment_path)
        {
        }

        bool COpenGLShaderDriver::InitDriver(CKongDeviceWin32* device)
        {
            if (!COpenGLDriver::InitDriver(device))
            {
                return false;
            }

            shader_helper_ = new COpenGLShaderHelper(io_, vertex_path_, fragment_path_);

            glGenVertexArrays(1, &vao_);
            glGenBuffers(1, &vbo_);
            glGenBuffers(1, &ebo_);

            return shader_helper_ != nullptr;
        }

        void COpenGLShaderDriver::SetTransform(E_TRANSFORMATION_STATE state, const core::Matrixf& mat)
        {
            matrices_[state] = mat;

            switch (state)
            {
            case ETS_VIEW:
                shader_helper_->SetMatrix4(std::string("view_transform"), mat);
                break;
            case ETS_WORLD:
                //// we have to update the clip planes to the latest view matrix
                //for (u32 i = 0; i < MaxUserClipPlanes; ++i)
                //if (UserClipPlanes[i].Enabled)
                //    uploadClipPlane(i);

                // now the real model-view matrix
                shader_helper_->SetMatrix4(std::string("world_transform"), mat);
                break;
            case ETS_PROJECTION:
                shader_helper_->SetMatrix4(std::string("project_transform"), mat);
                break;
            case ETS_COUNT:
            default:
                return;
            }
        }

        void COpenGLShaderDriver::DrawMeshBuffer(const scene::IMeshBuffer* mesh_buffer)
        {
            const S3DVertex *vertices = static_cast<const S3DVertex*>(mesh_buffer->GetVertices());
            u32 vertice_count = mesh_buffer->GetVertexCount();
            const u16 *indices = mesh_buffer->GetIndices();
            const GLsizei indices_count = mesh_buffer->GetIndexCount();

            glBindVertexArray(vao_);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(S3DVertex) * vertice_count, vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * indices_count, indices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(S3DVertex), reinterpret_cast<void *>(offsetof(S3DVertex, pos_)));
            glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(S3DVertex), reinterpret_cast<void *>(offsetof(S3DVertex, color_)));
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(S3DVertex), reinterpret_cast<void *>(offsetof(S3DVertex, texcoord_)));
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);

            shader_helper_->Use();
            glBindVertexArray(vao_);
            glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_SHORT, nullptr);
            glBindVertexArray(0);
        }

        bool COpenGLShaderDriver::SetActiveTexture(u32 stage, const video::ITexture* texture)
        {
            if (current_texture_[stage] == texture)
                return true;

            current_texture_.Set(stage, texture);

            if (!texture)
            {
                glDisable(GL_TEXTURE_2D);
                return true;
            }
            else
            {
                if (texture->GetDriverType() != EDT_OPENGL)
                {
                    glDisable(GL_TEXTURE_2D);
                    current_texture_.Set(stage, nullptr);
                    //os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
                    return false;
                }

                shader_helper_->Use();
                glEnable(GL_TEXTURE_2D);
                glActiveTexture(GL_TEXTURE0 + stage);
                glBindTexture(GL_TEXTURE_2D, dynamic_cast<const COpenGLTexture*>(texture)->GetOpenGLTextureName());
                core::stringc str = core::stringc("texture") + core::stringc(stage);
                shader_helper_->SetInt(str.c_str(), stage);
            }
            return true;
        }
    } // end namespace video
} // end namespace kong

namespace kong
{
    namespace video
    {
#ifdef _KONG_COMPILE_WITH_OPENGL_
        IVideoDriver* CreateOpenGLShaderDriver(const kong::SKongCreationParameters& params,
            io::IFileSystem* io, CKongDeviceWin32* device)
        {
            COpenGLShaderDriver *driver = new COpenGLShaderDriver(params, io, device);
#ifdef _KONG_COMPILE_WITH_OPENGL_
            if (!driver->InitDriver(device))
            {
                return nullptr;
            }
            return driver;
#else
            return nullptr;
#endif
        }
#endif
    }
}