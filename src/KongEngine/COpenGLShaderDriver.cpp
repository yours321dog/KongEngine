// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "COpenGLShaderDriver.h"
#include "GL/glew.h"
#include "S3DVertex.h"
#include "IMeshBuffer.h"
#include "COpenGLTexture.h"
#include "COpenGLShaderHelper.h"

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

            if (shader_helper_ == nullptr)
            {
                return false;
            }

            shader_helper_->Use();
            shader_helper_->SetBool("texture0_on", false);

            return true;
        }

        void COpenGLShaderDriver::SetTransform(u32 state, const core::Matrixf& mat)
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

            core::stringc str_on = core::stringc("texture") + core::stringc(stage) + "_on";
            
            shader_helper_->Use();
            if (!texture)
            {
                Disable(SL_TEXTURE0 + stage);
                return true;
            }
            else
            {
                if (texture->GetDriverType() != EDT_OPENGL)
                {
                    Enable(SL_TEXTURE0 + stage);
                    current_texture_.Set(stage, nullptr);
                    //os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
                    return false;
                }

                glActiveTexture(GL_TEXTURE0 + stage);
                glBindTexture(GL_TEXTURE_2D, dynamic_cast<const COpenGLTexture*>(texture)->GetOpenGLTextureName());
                shader_helper_->SetInt(GetUniformName(SL_TEXTURE0 + stage), stage);
                shader_helper_->SetBool(str_on.c_str(), true);
            }
            return true;
        }

        void COpenGLShaderDriver::deleteAllDynamicLights()
        {
            for (u32 i = 0; i < max_support_lights_; i++)
            {
                Disable(SL_LIGHT0 + i);
            }

            COpenGLDriver::deleteAllDynamicLights();
        }

        s32 COpenGLShaderDriver::addDynamicLight(const SLight& light)
        {


            return COpenGLDriver::addDynamicLight(light);
        }

        void COpenGLShaderDriver::UpdateMaxSupportLights()
        {
            max_support_lights_ = 4;
        }

        void COpenGLShaderDriver::SetMaterialUniform(s32 material_val_type, void *val) const
        {
            if (material_val_type >= SL_MATERIAL_COUNT || material_val_type < 0)
            {
                return;
            }
            switch (material_val_type)
            {
            case SL_MAT_AMBIENT:
            case SL_MAT_DIFFUSE:
            case SL_MAT_SPECULAR:
            case SL_MAT_EMISSIVE:
                shader_helper_->SetVec4(material_uniform_name[material_val_type], static_cast<f32 *>(val));
                break;
            case SL_MAT_SHININESS:
                shader_helper_->SetFloat(material_uniform_name[material_val_type], *static_cast<f32 *>(val));
            default: break;
            }
            
        }

        void COpenGLShaderDriver::SetLightUniform(s32 light_idx, s32 light_val_type, void* val) const
        {
            if (light_idx < SL_LIGHT0 || light_idx >= SL_LIGHT0 + max_support_lights_ || 
                light_val_type < 0 || light_val_type >= SL_LIGHT_COUNT)
            {
                return;
            }

            core::stringc str = core::stringc(light_idx) + light_uniform_name[light_val_type];

            switch (light_val_type)
            {
            case SL_LIGHT_POSITION:
            case SL_LIGHT_DIRECTION:
            case SL_LIGHT_AMBIENT:
            case SL_LIGHT_DIFFUSE:
            case SL_LIGHT_SPECULAR:
            case SL_LIGHT_ATTENUATION:
                shader_helper_->SetVec4(str.c_str(), static_cast<f32 *>(val));
                break;
            case SL_LIGHT_EXPONENT:
            case SL_LIGHT_CUTOFF:
                shader_helper_->SetFloat(str.c_str(), *static_cast<f32 *>(val));
            default: break;
            }
        }

        void COpenGLShaderDriver::Enable(s32 idx) const
        {
            if (idx < 0 || idx >= SL_COUNT)
            {
                return;
            }

            core::stringc str = core::stringc(shader_uniform_name[idx]) + "_on";
            shader_helper_->SetBool(str.c_str(), true);
        }

        void COpenGLShaderDriver::Disable(s32 idx) const
        {
            if (idx < 0 || idx >= SL_COUNT)
            {
                return;
            }

            core::stringc str = core::stringc(shader_uniform_name[idx]) + "_on";
            shader_helper_->SetBool(str.c_str(), false);
        }

        const c8* COpenGLShaderDriver::GetUniformName(s32 idx) const
        {
            if (idx < 0 || idx >= SL_COUNT)
            {
                return nullptr;
            }

            return shader_uniform_name[idx];
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