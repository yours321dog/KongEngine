// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _COPENGLDEFERREDSHADERDRIVER_H_
#define _COPENGLDEFERREDSHADERDRIVER_H_

#include "COpenGLShaderDriver.h"
#include "IShaderHelper.h"

namespace kong
{
    namespace video
    {
        class COpenGLFBODeferredTexture;

        class COpenGLDeferredShaderDriver : public COpenGLShaderDriver
        {
        public:
            COpenGLDeferredShaderDriver(const SKongCreationParameters &params, io::IFileSystem* file_system, CKongDeviceWin32* device);

            virtual ~COpenGLDeferredShaderDriver();

            bool InitDriver(CKongDeviceWin32 *device) override;

            //! Render first pass for deferred render
            void RenderFirstPass() override;

            //! Render second pass for deferred render
            void RenderSecondPass() override;

            //! Enable deferred post render texture
            void EnablePostRenderTexture(u32 idx) const;

            void ActivateDynamicLights() override;

            //! Set main light, used for shadow rendering
            void SetMainLight(const SLight& light) override;

        protected:
            void SetLightUniform(s32 light_idx, s32 light_val_type, const void *val) const override;
            void SetLightUniform(s32 light_idx, s32 light_val_type, f32 val) const override;

            IShaderHelper *deferred_post_shader_helper_;
            IShaderHelper *deferred_base_shader_helper_;

            // render textures
            COpenGLFBODeferredTexture *frame_buffers_;

            u32 nr_lights_;
        };
    } // end namespace video
} // end namespace video

#endif