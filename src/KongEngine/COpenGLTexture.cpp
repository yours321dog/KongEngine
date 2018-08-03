// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Kong Engine".

#include "KongCompileConfig.h"
#include <GL/glew.h>
#include "os.h"
#include <iostream>
#include <cassert>

#ifdef _KONG_COMPILE_WITH_OPENGL_

#include "KongTypes.h"
#include "COpenGLTexture.h"
#include "COpenGLDriver.h"
//#include "os.h"
#include "CColorConverter.h"

#include "KongString.h"

namespace kong
{
    namespace video
    {

        void CheckErrorCode()
        {
            // check OpenGL error
            GLenum err;
            while ((err = glGetError()) != GL_NO_ERROR) {
                std::cerr << "OpenGL error: " << err << std::endl;
            }

            //assert(err == GL_NO_ERROR);
        }

        //! constructor for usual textures
        COpenGLTexture::COpenGLTexture(IImage* origImage, const io::path& name, void* mipmapData, COpenGLDriver* driver)
            : ITexture(name), ColorFormat(ECF_A8R8G8B8), driver_(driver), image_(nullptr), MipImage(nullptr),
            texture_name_(0), internal_format_(GL_RGBA), pixel_format_(GL_BGRA_EXT),
            pixel_type_(GL_UNSIGNED_BYTE), MipLevelStored(0), MipmapLegacyMode(true),
            is_render_target_(false), AutomaticMipmapUpdate(false),
            ReadOnlyLock(false), KeepImage(true)
        {
            GetImageValues(origImage);

            glGenTextures(1, &texture_name_);

            image_ = driver_->CreateImage(ColorFormat, image_size_);
            origImage->CopyTo(image_);

            UploadTexture(true, mipmapData);

            if (!KeepImage)
            {
                delete image_;
                image_ = nullptr;
            }
        }


        //! constructor for basic setup (only for derived classes)
        COpenGLTexture::COpenGLTexture(const io::path& name, COpenGLDriver* driver)
            : ITexture(name), ColorFormat(ECF_A8R8G8B8), driver_(driver), image_(0), MipImage(0),
            texture_name_(0), internal_format_(GL_RGBA), pixel_format_(GL_BGRA_EXT),
            pixel_type_(GL_UNSIGNED_BYTE), MipLevelStored(0), has_mip_maps_(true),
            MipmapLegacyMode(true), is_render_target_(false), AutomaticMipmapUpdate(false),
            ReadOnlyLock(false), KeepImage(true)
        {
#ifdef _DEBUG
            //setDebugName("COpenGLTexture");
#endif
        }


        //! destructor
        COpenGLTexture::~COpenGLTexture()
        {
            if (texture_name_)
                glDeleteTextures(1, &texture_name_);
            delete image_;
        }


        //! Choose best matching color format, based on texture creation flags
        ECOLOR_FORMAT COpenGLTexture::GetBestColorFormat(ECOLOR_FORMAT format)
        {
            return format;
        }


        //! Get opengl values for the GPU texture storage
        GLint COpenGLTexture::getOpenGLFormatAndParametersFromColorFormat(ECOLOR_FORMAT format,
            GLint& filtering,
            GLenum& colorformat,
            GLenum& type)
        {
            // default
            filtering = GL_LINEAR;
            colorformat = GL_RGBA;
            type = GL_UNSIGNED_BYTE;
            GLenum internalformat = GL_RGBA;

            switch (format)
            {
            case ECF_A1R5G5B5:
                colorformat = GL_BGRA_EXT;
                type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
                internalformat = GL_RGBA;
                break;
            case ECF_R5G6B5:
                colorformat = GL_RGB;
                type = GL_UNSIGNED_SHORT_5_6_5;
                internalformat = GL_RGB;
                break;
            case ECF_R8G8B8:
                colorformat = GL_BGR;
                type = GL_UNSIGNED_BYTE;
                internalformat = GL_RGB;
                break;
            case ECF_A8R8G8B8:
                colorformat = GL_BGRA_EXT;
                //if (Driver->Version > 101)
                type = GL_UNSIGNED_INT_8_8_8_8_REV;
                internalformat = GL_RGBA;
                break;
                // Floating Point texture formats. Thanks to Patryk "Nadro" Nadrowski.
            case ECF_R16F:
            {
#ifdef GL_ARB_texture_rg
                filtering = GL_NEAREST;
                colorformat = GL_RED;
                type = GL_FLOAT;

                internalformat = GL_R16F;
#else
                ColorFormat = ECF_A8R8G8B8;
                internalformat = GL_RGB8;
#endif
            }
                break;
            case ECF_G16R16F:
            {
#ifdef GL_ARB_texture_rg
                filtering = GL_NEAREST;
                colorformat = GL_RG;
                type = GL_FLOAT;

                internalformat = GL_RG16F;
#else
                ColorFormat = ECF_A8R8G8B8;
                internalformat = GL_RGB8;
#endif
            }
                break;
            case ECF_A16B16G16R16F:
            {
#ifdef GL_ARB_texture_rg
                filtering = GL_NEAREST;
                colorformat = GL_RGBA;
                type = GL_FLOAT;

                internalformat = GL_RGBA16F_ARB;
#else
                ColorFormat = ECF_A8R8G8B8;
                internalformat = GL_RGBA8;
#endif
            }
                break;
            case ECF_R32F:
            {
#ifdef GL_ARB_texture_rg
                filtering = GL_NEAREST;
                colorformat = GL_RED;
                type = GL_FLOAT;

                internalformat = GL_R32F;
#else
                ColorFormat = ECF_A8R8G8B8;
                internalformat = GL_RGB8;
#endif
            }
                break;
            case ECF_G32R32F:
            {
#ifdef GL_ARB_texture_rg
                filtering = GL_NEAREST;
                colorformat = GL_RG;
                type = GL_FLOAT;

                internalformat = GL_RG32F;
#else
                ColorFormat = ECF_A8R8G8B8;
                internalformat = GL_RGB8;
#endif
            }
                break;
            case ECF_A32B32G32R32F:
            {
#ifdef GL_ARB_texture_float
                filtering = GL_NEAREST;
                colorformat = GL_RGBA;
                type = GL_FLOAT;

                internalformat = GL_RGBA32F_ARB;
#else
                ColorFormat = ECF_A8R8G8B8;
                internalformat = GL_RGBA8;
#endif
            }
                break;
            default:
            {
                //os::Printer::log("Unsupported texture format", ELL_ERROR);
                internalformat = GL_RGBA8;
            }
            }
#if defined(GL_ARB_framebuffer_sRGB) || defined(GL_EXT_framebuffer_sRGB)
            //if (Driver->params_.HandleSRGB)
            //{
            //    if (internalformat == GL_RGBA)
            //        internalformat = GL_SRGB_ALPHA_EXT;
            //    else if (internalformat == GL_RGB)
            //        internalformat = GL_SRGB_EXT;
            //}
#endif
            return internalformat;
        }


        // prepare values ImageSize, TextureSize, and ColorFormat based on image
        void COpenGLTexture::GetImageValues(IImage* image)
        {
            if (image == nullptr)
            {
                return;
            }

            image_size_ = image->GetDimension();
            texture_size_ = image_size_;
        }


        //! copies the the texture into an open gl texture.
        void COpenGLTexture::UploadTexture(bool newTexture, void* mipmapData, u32 level)
        {
            void *data = image_->Lock();
            GLint filtering;
            GLenum colorformat;
            GLenum type;
            const GLenum internalformat = getOpenGLFormatAndParametersFromColorFormat(ColorFormat, filtering, colorformat, type);

            glBindTexture(GL_TEXTURE_2D, texture_name_);
            // set the render filter and interpolation
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            if (data != nullptr)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, internalformat, image_size_.width_, image_size_.height_, 0, colorformat, type, data);
            }
            image_->Unlock();
        }


        //! lock function
        void* COpenGLTexture::Lock(E_TEXTURE_LOCK_MODE mode, u32 mipmapLevel)
        {
            glBindTexture(GL_TEXTURE_2D, texture_name_);
            return nullptr;
        }


        //! unlock function
        void COpenGLTexture::Unlock()
        {
            glBindTexture(GL_TEXTURE_2D, 0);
        }


        //! Returns size of the original image.
        const core::Dimension2d<u32>& COpenGLTexture::GetOriginalSize() const
        {
            return image_size_;
        }


        //! Returns size of the texture.
        const core::Dimension2d<u32>& COpenGLTexture::GetSize() const
        {
            return texture_size_;
        }


        //! returns driver type of texture, i.e. the driver, which created the texture
        E_DRIVER_TYPE COpenGLTexture::GetDriverType() const
        {
            return EDT_OPENGL;
        }


        //! returns color format of texture
        ECOLOR_FORMAT COpenGLTexture::GetColorFormat() const
        {
            return ColorFormat;
        }


        //! returns pitch of texture (in bytes)
        u32 COpenGLTexture::GetPitch() const
        {
            if (image_)
                return image_->GetPitch();
            else
                return 0;
        }


        //! return open gl texture name
        GLuint COpenGLTexture::GetOpenGLTextureName() const
        {
            return texture_name_;
        }


        //! Returns whether this texture has mipmaps
        bool COpenGLTexture::HasMipMaps() const
        {
            return has_mip_maps_;
        }


        //! Regenerates the mip map levels of the texture. Useful after locking and
        //! modifying the texture
        void COpenGLTexture::RegenerateMipMapLevels(void* mipmapData)
        {
        }


        bool COpenGLTexture::IsRenderTarget() const
        {
            return is_render_target_;
        }


        void COpenGLTexture::SetIsRenderTarget(bool isTarget)
        {
            is_render_target_ = isTarget;
        }


        bool COpenGLTexture::isFrameBufferObject() const
        {
            return false;
        }


        //! Bind Render Target Texture
        void COpenGLTexture::bindRTT()
        {
        }


        //! Unbind Render Target Texture
        void COpenGLTexture::unbindRTT()
        {
        }


        /* FBO Textures */

        // helper function for render to texture
        static bool checkFBOStatus(COpenGLDriver* Driver);

        //! RTT ColorFrameBuffer constructor
        COpenGLFBOTexture::COpenGLFBOTexture(const core::Dimension2d<u32>& size,
            const io::path& name, COpenGLDriver* driver, bool depth_test, ECOLOR_FORMAT format)
            : COpenGLTexture(name, driver), depth_texture_(0), color_frame_buffer_(0), depth_render_buffer_(0)
        {
            image_size_ = size;
            texture_size_ = size;

            if (ECF_UNKNOWN == format)
                format = GetBestColorFormat(driver->GetColorFormat());

            ColorFormat = format;

            GLint filtering_type;
            internal_format_ = getOpenGLFormatAndParametersFromColorFormat(format, filtering_type, pixel_format_, pixel_type_);

            has_mip_maps_ = false;
            is_render_target_ = true;

            // generate frame buffer
            glGenFramebuffers(1, &color_frame_buffer_);
            COpenGLFBOTexture::bindRTT();

            // generate color texture
            glGenTextures(1, &texture_name_);
            glBindTexture(GL_TEXTURE_2D, texture_name_);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering_type);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering_type);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, internal_format_, image_size_.width_,
                image_size_.height_, 0, pixel_format_, pixel_type_, 0);

            if (depth_test)
            {
                // The depth buffer
                glGenRenderbuffers(1, &depth_render_buffer_);
                glBindRenderbuffer(GL_RENDERBUFFER, depth_render_buffer_);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, image_size_.width_, image_size_.height_);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_render_buffer_);
            }

            // attach color texture to frame buffer
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_name_, 0);

#ifdef _DEBUG
            checkFBOStatus(driver);
#endif
            COpenGLFBOTexture::unbindRTT();
        }


        //! destructor
        COpenGLFBOTexture::~COpenGLFBOTexture()
        {
            glDeleteFramebuffers(1, &color_frame_buffer_);
        }


        bool COpenGLFBOTexture::isFrameBufferObject() const
        {
            return true;
        }


        //! Bind Render Target Texture
        void COpenGLFBOTexture::bindRTT()
        {
            if (color_frame_buffer_ != 0)
                glBindFramebuffer(GL_FRAMEBUFFER, color_frame_buffer_);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
        }


        //! Unbind Render Target Texture
        void COpenGLFBOTexture::unbindRTT()
        {
            if (color_frame_buffer_ != 0)
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }


        /* FBO Depth Textures */

        //! RTT DepthBuffer constructor
        COpenGLFBODepthTexture::COpenGLFBODepthTexture(
            const core::Dimension2d<u32>& size,
            const io::path& name,
            COpenGLDriver* driver,
            bool useStencil)
            : COpenGLTexture(name, driver), DepthRenderBuffer(0),
            StencilRenderBuffer(0), UseStencil(useStencil)
        {
#ifdef _DEBUG
            //setDebugName("COpenGLTextureFBO_Depth");
#endif

            image_size_ = size;
            texture_size_ = size;
            internal_format_ = GL_RGBA;
            pixel_format_ = GL_RGBA;
            pixel_format_ = GL_UNSIGNED_BYTE;
            has_mip_maps_ = false;

            if (useStencil)
            {
                glGenTextures(1, &DepthRenderBuffer);
                glBindTexture(GL_TEXTURE_2D, DepthRenderBuffer);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                {
                    // generate depth texture
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, image_size_.width_,
                        image_size_.height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

                    //// generate stencil texture
                    //glGenTextures(1, &StencilRenderBuffer);
                    //glBindTexture(GL_TEXTURE_2D, StencilRenderBuffer);
                    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    //glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX, image_size_.width_,
                    //    image_size_.height_, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, 0);
                }
            }
            else
            {
                // generate depth buffer
                glGenRenderbuffers(1, &DepthRenderBuffer);
                glBindRenderbuffer(GL_RENDERBUFFER, DepthRenderBuffer);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, image_size_.width_, image_size_.height_);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthRenderBuffer);
            }

            texture_name_ = DepthRenderBuffer;
        }


        //! destructor
        COpenGLFBODepthTexture::~COpenGLFBODepthTexture()
        {
            if (DepthRenderBuffer && UseStencil)
                glDeleteTextures(1, &DepthRenderBuffer);
            else
                glDeleteRenderbuffers(1, &DepthRenderBuffer);
            if (StencilRenderBuffer && StencilRenderBuffer != DepthRenderBuffer)
                glDeleteTextures(1, &StencilRenderBuffer);
        }

        //! Bind Render Target Texture
        void COpenGLFBODepthTexture::bindRTT()
        {
        }


        //! Unbind Render Target Texture
        void COpenGLFBODepthTexture::unbindRTT()
        {
        }

        //combine depth texture and rtt
        bool COpenGLFBODepthTexture::attach(ITexture* renderTex)
        {
            if (!renderTex)
                return false;
            video::COpenGLFBOTexture* rtt = static_cast<video::COpenGLFBOTexture*>(renderTex);
            rtt->bindRTT();
            if (UseStencil)
            {
                // attach stencil texture to stencil buffer
                //glFramebufferTexture2D(GL_FRAMEBUFFER,
                //    GL_STENCIL_ATTACHMENT,
                //    GL_TEXTURE_2D,
                //    StencilRenderBuffer,
                //    0);

                // attach depth texture to depth buffer
                glFramebufferTexture2D(GL_FRAMEBUFFER,
                    GL_DEPTH_ATTACHMENT,
                    GL_TEXTURE_2D,
                    DepthRenderBuffer,
                    0);
            }
            else
            {
                // attach depth renderbuffer to depth buffer
                glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT,
                    GL_DEPTH_ATTACHMENT_EXT,
                    GL_RENDERBUFFER_EXT,
                    DepthRenderBuffer);
            }
            // check the status
            if (!checkFBOStatus(driver_))
            {
                os::Printer::log("FBO incomplete");
                return false;
            }
            rtt->depth_texture_ = this;
            //grab(); // grab the depth buffer, not the RTT
            rtt->unbindRTT();
            return true;
        }

        

        COpenGLFBODeferredTexture::COpenGLFBODeferredTexture(const core::Dimension2d<u32>& size, const io::path& name, COpenGLDriver* driver)
            : COpenGLTexture(name, driver)
        {
            image_size_ = size;
            texture_size_ = size;

            internal_format_ = GL_RGBA;
            pixel_format_ = GL_RGBA;
            pixel_format_ = GL_UNSIGNED_BYTE;
            has_mip_maps_ = false;

            // generate frame buffer
            glGenFramebuffers(1, &color_frame_buffer_);

            glBindFramebuffer(GL_FRAMEBUFFER, color_frame_buffer_);

            // check OpenGL error
            GLenum err;
            while ((err = glGetError()) != GL_NO_ERROR) {
                std::cerr << "OpenGL error: " << err << std::endl;
            }

            // position texture
            //glGenTextures(1, &texture_names_[EGBT_POSTION]);
            //glBindTexture(GL_TEXTURE_2D, texture_names_[EGBT_POSTION]);
            ////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering_type);
            ////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering_type);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, image_size_.width_, image_size_.height_, 0, GL_RGBA, GL_FLOAT, 0);
            //glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + EGBT_POSTION, texture_names_[EGBT_POSTION], 0);
            //attachments_[EGBT_POSTION] = GL_COLOR_ATTACHMENT0 + EGBT_POSTION;
            //BindFramebufferTexture(EGBT_POSTION, GL_RGB32F, GL_RGB, GL_FLOAT);
            BindFramebufferTexture(EGBT_POSTION, GL_RGB16F, GL_RGB, GL_HALF_FLOAT);

            // normal texture
            //glGenTextures(1, &texture_names_[EGBT_NORMAL]);
            //glBindTexture(GL_TEXTURE_2D, texture_names_[EGBT_NORMAL]);
            ////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering_type);
            ////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering_type);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, image_size_.width_, image_size_.height_, 0, GL_RGBA, GL_FLOAT, 0);
            //glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, texture_names_[EGBT_NORMAL], 0);
            //attachments_[1] = GL_COLOR_ATTACHMENT1;
            //BindFramebufferTexture(EGBT_NORMAL, GL_RGB32F, GL_RGB, GL_FLOAT);
            BindFramebufferTexture(EGBT_NORMAL, GL_RGB16F, GL_RGB, GL_HALF_FLOAT);

            // diffuse color texture
            //glGenTextures(1, &texture_names_[EGBT_DIFFUSE]);
            //glBindTexture(GL_TEXTURE_2D, texture_names_[EGBT_DIFFUSE]);
            ////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering_type);
            ////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering_type);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_size_.width_, image_size_.height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
            //glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, texture_names_[EGBT_DIFFUSE], 0);
            //attachments_[2] = GL_COLOR_ATTACHMENT2;
            BindFramebufferTexture(EGBT_DIFFUSE, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

            glGenRenderbuffers(1, &depth_texture_name_);
            glBindRenderbuffer(GL_RENDERBUFFER, depth_texture_name_);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, image_size_.width_, image_size_.height_);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_texture_name_);

            //// depth buffer
            //glGenTextures(1, &depth_texture_name_);
            //glBindTexture(GL_TEXTURE_2D, depth_texture_name_);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, image_size_.width_, image_size_.height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
            //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture_name_, 0);

#ifdef _DEBUG
            checkFBOStatus(driver);
#endif

            COpenGLFBODeferredTexture::unbindRTT();
        }

        COpenGLFBODeferredTexture::~COpenGLFBODeferredTexture()
        {
            glDeleteTextures(EGBT_COUNT, texture_names_);
            glDeleteRenderbuffers(1, &depth_texture_name_);
        }

        bool COpenGLFBODeferredTexture::isFrameBufferObject() const
        {
            return true;
        }

        void COpenGLFBODeferredTexture::bindRTT()
        {
            if (color_frame_buffer_ != 0)
                glBindFramebuffer(GL_FRAMEBUFFER, color_frame_buffer_);
            glDrawBuffers(EGBT_COUNT, attachments_);

        }

        void COpenGLFBODeferredTexture::unbindRTT()
        {
            if (color_frame_buffer_ != 0)
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        GLuint COpenGLFBODeferredTexture::GetTextureName(u32 idx)
        {
            if (idx < EGBT_COUNT)
            {
                return texture_names_[idx];
            }
            return texture_names_[0];
        }

        void COpenGLFBODeferredTexture::BindFramebufferTexture(u32 idx, GLint internal_format, GLenum pixel_format, GLenum pixel_type)
        {
            glGenTextures(1, &texture_names_[idx]);
            glBindTexture(GL_TEXTURE_2D, texture_names_[idx]);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering_type);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering_type);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, internal_format, image_size_.width_, image_size_.height_, 0, pixel_format, pixel_type, 0);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + idx, texture_names_[idx], 0);
            attachments_[idx] = GL_COLOR_ATTACHMENT0 + idx;

            // check OpenGL error
            GLenum err;
            while ((err = glGetError()) != GL_NO_ERROR) {
                std::cerr << "idx : " << idx << ", OpenGL error: " << err << std::endl;
            }
        }

        bool checkFBOStatus(COpenGLDriver* Driver)
        {
#ifdef GL_EXT_framebuffer_object
            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

            switch (status)
            {
                //Our FBO is perfect, return true
            case GL_FRAMEBUFFER_COMPLETE_EXT:
                return true;

            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
                os::Printer::log("FBO has invalid read buffer", ELL_ERROR);
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
                os::Printer::log("FBO has invalid draw buffer", ELL_ERROR);
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
                os::Printer::log("FBO has one or several incomplete image attachments", ELL_ERROR);
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
                os::Printer::log("FBO has one or several image attachments with different internal formats", ELL_ERROR);
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
                os::Printer::log("FBO has one or several image attachments with different dimensions", ELL_ERROR);
                break;

                // not part of fbo_object anymore, but won't harm as it is just a return value
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT
            case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
                os::Printer::log("FBO has a duplicate image attachment", ELL_ERROR);
                break;
#endif

            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
                os::Printer::log("FBO missing an image attachment", ELL_ERROR);
                break;

#ifdef GL_EXT_framebuffer_multisample
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT:
                os::Printer::log("FBO wrong multisample setup", ELL_ERROR);
                break;
#endif

            case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
                os::Printer::log("FBO format unsupported", ELL_ERROR);
                break;

            default:
                break;
            }
#endif
            os::Printer::log("FBO error", ELL_ERROR);
            //	_IRR_DEBUG_BREAK_IF(true);
            return false;
        }


    } // end namespace video
} // end namespace irr

#endif // _KONG_COMPILE_WITH_OPENGL_

