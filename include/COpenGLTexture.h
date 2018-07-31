// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef _COPENGLTEXTURE_H_
#define _COPENGLTEXTURE_H_

#include "ITexture.h"
#include "IImage.h"

#include "KongCompileConfig.h"
#ifdef _KONG_COMPILE_WITH_OPENGL_

#if defined(_KONG_OPENGL_USE_EXTPOINTER_)
#define GL_GLEXT_LEGACY 1
#else
#define GL_GLEXT_PROTOTYPES 1
#endif
#ifdef _KONG_WINDOWS_API_
// include windows headers for HWND
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>
#ifdef _MSC_VER
#pragma comment(lib, "OpenGL32.lib")
#endif
#elif defined(_KONG_OSX_PLATFORM_)
#include <OpenGL/gl.h>
#elif defined(_KONG_COMPILE_WITH_SDL_DEVICE_)
#define NO_SDL_GLEXT
#include <SDL/SDL_video.h>
#include <SDL/SDL_opengl.h>
#else
#if defined(_KONG_OSX_PLATFORM_)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#endif


namespace kong
{
    namespace video
    {

        class COpenGLDriver;
        //! OpenGL texture.
        class COpenGLTexture : public ITexture
        {
        public:

            //! constructor
            COpenGLTexture(IImage* surface, const io::path& name, void* mipmapData = 0, COpenGLDriver* driver = 0);

            //! destructor
            virtual ~COpenGLTexture();

            //! lock function
            virtual void* Lock(E_TEXTURE_LOCK_MODE mode = ETLM_READ_WRITE, u32 mipmapLevel = 0);

            //! unlock function
            virtual void Unlock();

            //! Returns original size of the texture (image).
            virtual const core::Dimension2d<u32>& GetOriginalSize() const;

            //! Returns size of the texture.
            virtual const core::Dimension2d<u32>& GetSize() const;

            //! returns driver type of texture (=the driver, that created it)
            virtual E_DRIVER_TYPE GetDriverType() const;

            //! returns color format of texture
            virtual ECOLOR_FORMAT GetColorFormat() const;

            //! returns pitch of texture (in bytes)
            virtual u32 GetPitch() const;

            //! return open gl texture name
            GLuint GetOpenGLTextureName() const;

            //! return whether this texture has mipmaps
            virtual bool HasMipMaps() const;

            //! Regenerates the mip map levels of the texture.
            /** Useful after locking and modifying the texture
            \param mipmapData Pointer to raw mipmap data, including all necessary mip levels, in the same format as the main texture image. If not set the mipmaps are derived from the main image. */
            virtual void RegenerateMipMapLevels(void* mipmapData = 0);

            //! Is it a render target?
            virtual bool IsRenderTarget() const;

            //! Is it a FrameBufferObject?
            virtual bool isFrameBufferObject() const;

            //! Bind RenderTargetTexture
            virtual void bindRTT();

            //! Unbind RenderTargetTexture
            virtual void unbindRTT();

            //! sets whether this texture is intended to be used as a render target.
            void SetIsRenderTarget(bool isTarget);

        protected:

            //! protected constructor with basic setup, no GL texture name created, for derived classes
            COpenGLTexture(const io::path& name, COpenGLDriver* driver);

            //! get the desired color format based on texture creation flags and the input format.
            ECOLOR_FORMAT GetBestColorFormat(ECOLOR_FORMAT format);

            //! Get the OpenGL color format parameters based on the given Irrlicht color format
            GLint getOpenGLFormatAndParametersFromColorFormat(
                ECOLOR_FORMAT format, GLint& filtering, GLenum& colorformat, GLenum& type);

            //! get important numbers of the image and hw texture
            void GetImageValues(IImage* image);

            //! copies the texture into an OpenGL texture.
            /** \param newTexture True if method is called for a newly created texture for the first time. Otherwise call with false to improve memory handling.
            \param mipmapData Pointer to raw mipmap data, including all necessary mip levels, in the same format as the main texture image.
            \param mipLevel If set to non-zero, only that specific miplevel is updated, using the MipImage member. */
            void UploadTexture(bool newTexture = false, void* mipmapData = 0, u32 mipLevel = 0);

            core::Dimension2d<u32> image_size_;
            core::Dimension2d<u32> texture_size_;
            ECOLOR_FORMAT ColorFormat;
            COpenGLDriver* driver_;
            IImage* image_;
            IImage* MipImage;

            GLuint texture_name_;
            GLint internal_format_;
            GLenum pixel_format_;
            GLenum pixel_type_;

            u8 MipLevelStored;
            bool has_mip_maps_;
            bool MipmapLegacyMode;
            bool is_render_target_;
            bool AutomaticMipmapUpdate;
            bool ReadOnlyLock;
            bool KeepImage;
        };

        //! OpenGL FBO texture.
        class COpenGLFBOTexture : public COpenGLTexture
        {
        public:

            //! FrameBufferObject constructor
            COpenGLFBOTexture(const core::Dimension2d<u32>& size, const io::path& name,
                COpenGLDriver* driver = 0, bool depth_test = false, ECOLOR_FORMAT format = ECF_UNKNOWN);

            //! destructor
            virtual ~COpenGLFBOTexture();

            //! Is it a FrameBufferObject?
            virtual bool isFrameBufferObject() const;

            //! Bind RenderTargetTexture
            virtual void bindRTT();

            //! Unbind RenderTargetTexture
            virtual void unbindRTT();

            ITexture* depth_texture_;
        protected:
            GLuint color_frame_buffer_;
            GLuint depth_render_buffer_;
        };


        //! OpenGL FBO depth texture.
        class COpenGLFBODepthTexture : public COpenGLTexture
        {
        public:
            //! FrameBufferObject depth constructor
            COpenGLFBODepthTexture(const core::Dimension2d<u32>& size, const io::path& name, COpenGLDriver* driver = 0, bool useStencil = false);

            //! destructor
            virtual ~COpenGLFBODepthTexture();

            //! Bind RenderTargetTexture
            virtual void bindRTT();

            //! Unbind RenderTargetTexture
            virtual void unbindRTT();

            bool attach(ITexture*);

        protected:
            GLuint DepthRenderBuffer;
            GLuint StencilRenderBuffer;
            bool UseStencil;
        };


    } // end namespace video
} // end namespace irr

#endif
#endif // _KONG_COMPILE_WITH_OPENGL_

