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

        protected:
            IShaderHelper *deferred_shader_helper_;

            // render textures
            COpenGLFBODeferredTexture *frame_buffers_;
        };
    } // end namespace video
} // end namespace video

#endif