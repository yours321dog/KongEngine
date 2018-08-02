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
              deferred_base_shader_helper_(nullptr), frame_buffers_(nullptr)
        {
        }

        COpenGLDeferredShaderDriver::~COpenGLDeferredShaderDriver()
        {
            delete deferred_post_shader_helper_;
            delete deferred_base_shader_helper_;
            delete frame_buffers_;
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

#ifdef _DEBUG 
            CheckErrorCode();
#endif

            return true;
        }

        void COpenGLDeferredShaderDriver::RenderFirstPass()
        {
            CheckErrorCode();
            shader_helper_ = deferred_base_shader_helper_;
            shader_helper_->Use();
            CheckErrorCode();

            if (frame_buffers_ != nullptr && frame_buffers_->isFrameBufferObject())
            {
                frame_buffers_->bindRTT();
            }
            CheckErrorCode();

            ClearBuffers(color_buffer_clear_, z_buffer_clear_, false, color_clear_);
            render_material_texture_on_ = true;
        }

        void COpenGLDeferredShaderDriver::RenderSecondPass()
        {
            CheckErrorCode();
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
            CheckErrorCode();
        }

        void COpenGLDeferredShaderDriver::EnablePostRenderTexture(u32 idx) const
        {
            deferred_post_shader_helper_->SetInt(GetUniformName(SL_DEFERRED_PASS_0 + idx), idx);
            glActiveTexture(GL_TEXTURE0 + idx);
            glBindTexture(GL_TEXTURE_2D, frame_buffers_->GetTextureName(idx));
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