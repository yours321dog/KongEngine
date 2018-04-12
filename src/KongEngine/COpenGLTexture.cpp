// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Kong Engine".

#include "KongCompileConfig.h"
#include <GL/glew.h>

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

        //! constructor for usual textures
        COpenGLTexture::COpenGLTexture(IImage* origImage, const io::path& name, void* mipmapData, COpenGLDriver* driver)
            : ITexture(name), ColorFormat(ECF_A8R8G8B8), driver_(driver), image_(nullptr), MipImage(nullptr),
            texture_name_(0), InternalFormat(GL_RGBA), PixelFormat(GL_BGRA_EXT),
            PixelType(GL_UNSIGNED_BYTE), MipLevelStored(0), MipmapLegacyMode(true),
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
            texture_name_(0), InternalFormat(GL_RGBA), PixelFormat(GL_BGRA_EXT),
            PixelType(GL_UNSIGNED_BYTE), MipLevelStored(0), has_mip_maps_(true),
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
        ECOLOR_FORMAT COpenGLTexture::getBestColorFormat(ECOLOR_FORMAT format)
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
            glBindTexture(GL_TEXTURE_2D, texture_name_);
            // set the render filter and interpolation
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            void *data = image_->Lock();
            if (data != nullptr)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA_EXT, image_size_.width_, image_size_.height_, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);
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
            const io::path& name, COpenGLDriver* driver,
            ECOLOR_FORMAT format)
            : COpenGLTexture(name, driver), DepthTexture(0), ColorFrameBuffer(0)
        {
        }


        //! destructor
        COpenGLFBOTexture::~COpenGLFBOTexture()
        {
        }


        bool COpenGLFBOTexture::isFrameBufferObject() const
        {
            return true;
        }


        //! Bind Render Target Texture
        void COpenGLFBOTexture::bindRTT()
        {
        }


        //! Unbind Render Target Texture
        void COpenGLFBOTexture::unbindRTT()
        {
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
        }


        //! destructor
        COpenGLFBODepthTexture::~COpenGLFBODepthTexture()
        {
        }


        //combine depth texture and rtt
        bool COpenGLFBODepthTexture::attach(ITexture* renderTex)
        {
            return false;
        }


        //! Bind Render Target Texture
        void COpenGLFBODepthTexture::bindRTT()
        {
        }


        //! Unbind Render Target Texture
        void COpenGLFBODepthTexture::unbindRTT()
        {
        }


        bool checkFBOStatus(COpenGLDriver* Driver)
        {
            return false;
        }


    } // end namespace video
} // end namespace irr

#endif // _KONG_COMPILE_WITH_OPENGL_

