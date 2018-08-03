// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".
#include <GL/glew.h>
#include "COpenGLDeferredShaderDriver.h"
#include "COpenGLShaderHelper.h"
#include "COpenGLTexture.h"


namespace kong
{
    namespace video
    {
        COpenGLDeferredShaderDriver::COpenGLDeferredShaderDriver(const SKongCreationParameters& params, io::IFileSystem* file_system, CKongDeviceWin32* device)
            : COpenGLShaderDriver(params, file_system, device), deferred_post_shader_helper_(nullptr),
              deferred_base_shader_helper_(nullptr), frame_buffers_(nullptr), nr_lights_(32)
        {
        }

        COpenGLDeferredShaderDriver::~COpenGLDeferredShaderDriver()
        {
            delete deferred_post_shader_helper_;
            delete deferred_base_shader_helper_;
            delete frame_buffers_;
        }

        bool COpenGLDeferredShaderDriver::BeginScene(bool back_buffer, bool z_buffer, SColor color)
        {
            if (!COpenGLDriver::BeginScene())
            {
                return false;
            }

            shader_helper_ = deferred_base_shader_helper_;
            shader_helper_->Use();
            return true;
        }

        bool COpenGLDeferredShaderDriver::InitDriver(CKongDeviceWin32* device)
        {
            if (!COpenGLShaderDriver::InitDriver(device))
            {
                return false;
            }

            deferred_base_shader_helper_ = new COpenGLShaderHelper(io_, io::SPath("./shaders/deferred_base.vs"), io::SPath("./shaders/deferred_base.fs"));
            deferred_post_shader_helper_ = new COpenGLShaderHelper(io_, io::SPath("./shaders/deferred_post.vs"), io::SPath("./shaders/deferred_post.fs"));

            shader_helper_ = deferred_base_shader_helper_;

            frame_buffers_ = new COpenGLFBODeferredTexture(params_.window_size_, io::path(), this);

            shader_helper_->Use();
            shader_helper_->SetBool("texture0_on", false);
            shader_helper_->SetBool("texture1_on", false);
            shader_helper_->SetBool("normal_mapping_on", false);

            return true;
        }

        void COpenGLDeferredShaderDriver::RenderFirstPass()
        {
            shader_helper_ = deferred_base_shader_helper_;
            shader_helper_->Use();

            if (frame_buffers_ != nullptr && frame_buffers_->isFrameBufferObject())
            {
                frame_buffers_->bindRTT();
            }

            ClearBuffers(color_buffer_clear_, z_buffer_clear_, false, color_clear_);
            render_material_texture_on_ = true;

            shader_helper_->SetBool("shadow_on", shadow_enable_);

        }

        void COpenGLDeferredShaderDriver::RenderSecondPass()
        {
            frame_buffers_->unbindRTT();
            shader_helper_ = deferred_post_shader_helper_;
            shader_helper_->Use();

            ////glCullFace(GL_BACK); // tells OpenGL to cull back faces (the sane default setting)
            ////glDisable(GL_CULL_FACE); // enables face culling    

            DeleteAllDynamicLights();
            render_material_texture_on_ = false;

            for (u32 i = 0; i < EGBT_COUNT; i++)
            {
                EnablePostRenderTexture(i);
            }

            // shadow texture
            shader_helper_->SetInt(GetUniformName(SL_DEFERRED_SHADOW), SL_DEFERRED_SHADOW - SL_DEFERRED_PASS_0);
            glActiveTexture(GL_TEXTURE0 + SL_DEFERRED_SHADOW - SL_DEFERRED_PASS_0);
            glBindTexture(GL_TEXTURE_2D, dynamic_cast<const COpenGLTexture*>(shadow_depth_texture_)->GetOpenGLTextureName());

            const f32 data[2] = { params_.window_size_.width_, params_.window_size_.height_ };
            deferred_post_shader_helper_->SetVec2("window_size", data);
            shader_helper_->SetBool("shadow_on", shadow_enable_);
        }

        void COpenGLDeferredShaderDriver::EnablePostRenderTexture(u32 idx) const
        {
            deferred_post_shader_helper_->SetInt(gbuffer_type_name[idx], idx);
            glActiveTexture(GL_TEXTURE0 + idx);
            glBindTexture(GL_TEXTURE_2D, frame_buffers_->GetTextureName(idx));
        }

        void COpenGLDeferredShaderDriver::ActivateDynamicLights()
        {
            shader_helper_->SetInt("lights_num", GetDynamicLightCount());
        }

        void COpenGLDeferredShaderDriver::SetMainLight(const SLight& light)
        {
            
        }

        void COpenGLDeferredShaderDriver::EnableShadow(bool flag)
        {
            COpenGLDriver::EnableShadow(flag);
        }

        void COpenGLDeferredShaderDriver::EndShadowRender()
        {            
            shadow_color_texture_->unbindRTT();

            DeleteAllDynamicLights();

            glViewport(0, 0, params_.window_size_.width_, params_.window_size_.height_);
        }

        void COpenGLDeferredShaderDriver::SetLightUniform(s32 light_idx, s32 light_val_type, const void* val) const
        {
            light_idx -= SL_LIGHT0;
            if (light_idx < 0 || light_idx >= nr_lights_)
                return;
            core::stringc str = core::stringc("lights[") + core::stringc(light_idx) + "]." + light_uniform_name[light_val_type];
            //os::Printer::print(str.c_str());

            switch (light_val_type)
            {
            case SL_LIGHT_POSITION:
            case SL_LIGHT_DIRECTION:
            case SL_LIGHT_AMBIENT:
            case SL_LIGHT_DIFFUSE:
            case SL_LIGHT_SPECULAR:
            case SL_LIGHT_ATTENUATION:
                shader_helper_->SetVec4(str.c_str(), static_cast<const f32 *>(val));
                break;
            case SL_LIGHT_EXPONENT:
            case SL_LIGHT_CUTOFF:
                shader_helper_->SetFloat(str.c_str(), *static_cast<const f32 *>(val));
            default: break;
            }
        }

        void COpenGLDeferredShaderDriver::SetLightUniform(s32 light_idx, s32 light_val_type, f32 val) const
        {
            light_idx -= SL_LIGHT0;
            if (light_idx < 0 || light_idx >= nr_lights_)
                return;
            core::stringc str = core::stringc("lights[") + core::stringc(light_idx) + "]." + light_uniform_name[light_val_type];

            shader_helper_->SetFloat(str.c_str(), val);
        }

;

        void COpenGLDeferredShaderDriver::DrawSpaceFillQuad()
        {
            u16 indices[6] = {
                0, 1, 2,
                0, 2, 3
            };

            f32 points[12] = {
                -1.f, -1.0, 1.0,
                1.0, -1.0, 1.0,
                1.0, 1.0, 1.0,
                -1.0, 1.0, 1.0
            };

            SMaterial mat_default;
            SetMaterial(mat_default);

            shader_helper_->Use();

            glBindVertexArray(vao_);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * 12, points, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * 6, indices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(f32) * 3, nullptr);

            glEnableVertexAttribArray(0);

            //glBindVertexArray(vao_);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

            //glBindVertexArray(0);

            CheckErrorCode();
        }
    } // end namespace video
} // end namespace kong

namespace kong
{
    namespace video
    {
#ifdef _KONG_COMPILE_WITH_OPENGL_
        IVideoDriver* CreateOpenGLDeferredShaderDriver(const kong::SKongCreationParameters& params,
            io::IFileSystem* io, CKongDeviceWin32* device)
        {
            COpenGLShaderDriver *driver = new COpenGLDeferredShaderDriver(params, io, device);
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