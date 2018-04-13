// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _COPENGLDRIVER_H_
#define _COPENGLDRIVER_H_

#include "IVideoDriver.h"
#include "SMaterial.h"
#include <Windows.h>
#include "SKongCreationParameters.h"
#include "IFileSystem.h"
#include "Array.h"
#include "IImageLoader.h"


namespace kong
{
    class CKongDeviceWin32;
}

namespace kong
{
    namespace video
    {
        class COpenGLDriver : public IVideoDriver
        {
        public:
            COpenGLDriver(const SKongCreationParameters &params, io::IFileSystem* io, CKongDeviceWin32* device);

            virtual ~COpenGLDriver() = default;

            virtual bool InitDriver(CKongDeviceWin32 *device);

            //! Sets a material.
            virtual void SetMaterial(const SMaterial& material);

            //! Applications must call this method before performing any rendering.
            bool BeginScene(bool back_buffer = true, bool z_buffer = true, SColor color = SColor(255, 0, 0, 0)) override;

            //! Presents the rendered image to the screen.
            bool EndScene() override;

            //! Sets transformation matrices.
            void SetTransform(E_TRANSFORMATION_STATE state, const core::Matrixf& mat) override;

            //! Draws a 3d line.
            void Draw3DLine(const core::Vector3Df& start,
                const core::Vector3Df& end, SColor color = SColor(255, 255, 255, 255)) override;

            //! Draws a 2d image using a color
            virtual void Draw2DImage(const video::ITexture* texture, const core::position2d<s32>& destPos,
                const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
                SColor color = SColor(255, 255, 255, 255), bool useAlphaChannelOfTexture = false);

            //! Draws a 2d image
            virtual void Draw2DImage(const video::ITexture* texture, const core::position2d<f32>& destPos,
                const core::rect<f32>& sourceRect, SColor color = SColor(255, 255, 255, 255));

            //! Draws a mesh buffer
            void DrawMeshBuffer(const scene::IMeshBuffer* mesh_buffer) override;

            //! Creates an empty software image.
            IImage* CreateImage(ECOLOR_FORMAT format, const core::Dimension2d<u32>& size) override;

            //! Creates a software image by converting it to given format from another image.
            IImage* CreateImage(ECOLOR_FORMAT format, IImage *imageToCopy) override;

            //! Creates a software image from a file.
            IImage* CreateImageFromFile(const io::path& filename) override;

            //! Creates a software image from a file.
            IImage* CreateImageFromFile(io::IReadFile* file) override;

            //! Check if the image is already loaded.
            ITexture* FindTexture(const io::path& filename) override;

            //! Get access to a named texture.
            ITexture* GetTexture(const io::path& filename) override;

            //! Get access to a named texture.
            ITexture* GetTexture(io::IReadFile* file) override;

            //! Creates an empty texture of specified size.
            ITexture* AddTexture(const core::Dimension2d<u32>& size,
                const io::path& name, ECOLOR_FORMAT format = ECF_A8R8G8B8) override;

            //! Creates a texture from an IImage.
            ITexture* AddTexture(const io::path& name, IImage* image, void* mipmapData = nullptr) override;

            //! returns a device dependent texture from a software surface (IImage)
            //! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
            virtual video::ITexture* createDeviceDependentTexture(IImage* surface, const io::path& name, void* mipmapData = nullptr);

            //! adds a surface, not loaded or created by the Irrlicht Engine
            void AddTexture(video::ITexture* surface);

            //! sets the current Texture
            //! Returns whether setting was a success or not.
            virtual bool SetActiveTexture(u32 stage, const video::ITexture* texture);

            //! disables all textures beginning with the optional fromStage parameter. Otherwise all texture stages are disabled.
            //! Returns whether disabling was successful or not.
            bool DisableTextures(u32 fromStage = 0);
        protected:
            //! enumeration for rendering modes such as 2d and 3d for minizing the switching of renderStates.
            enum E_RENDER_MODE
            {
                ERM_NONE = 0,	// no render state has been set yet.
                ERM_2D,		// 2d drawing rendermode
                ERM_3D		// 3d rendering mode
            };

            struct SSurface
            {
                video::ITexture* Surface;

                bool operator < (const SSurface& other) const
                {
                    return Surface->GetName() < other.Surface->GetName();
                }
            };

            struct SDummyTexture : public ITexture
            {
                SDummyTexture(const io::path& name) : ITexture(name), size(0, 0) {};

                virtual void* Lock(E_TEXTURE_LOCK_MODE mode = ETLM_READ_WRITE, u32 mipmapLevel = 0) { return 0; };
                virtual void Unlock(){}
                virtual const core::Dimension2d<u32>& GetOriginalSize() const { return size; }
                virtual const core::Dimension2d<u32>& GetSize() const { return size; }
                virtual E_DRIVER_TYPE GetDriverType() const { return video::EDT_NULL; }
                virtual ECOLOR_FORMAT GetColorFormat() const { return video::ECF_A1R5G5B5; };
                virtual u32 GetPitch() const { return 0; }
                virtual void RegenerateMipMapLevels(void* mipmapData = nullptr) {};
                core::Dimension2d<u32> size;
            };

            core::Array<SSurface> textures_;
            core::Array<video::IImageLoader*> surface_loader_;

            //! clears the zbuffer and color buffer
            void ClearBuffers(bool back_buffer, bool z_buffer, bool stencil_buffer, SColor color);

            //! opens the file and loads it into the surface
            video::ITexture* LoadTextureFromFile(io::IReadFile* file, const io::path& hashName = "");

            //! creates a transposed matrix in supplied GLfloat array to pass to OpenGL
            inline void GetGLMatrix(f32 gl_matrix[16], const core::Matrixf& m);
            inline void GetGLTextureMatrix(f32 gl_matrix[16], const core::Matrixf& m);

            class STextureStageCache
            {
                const ITexture* current_texture_[MATERIAL_MAX_TEXTURES];
            public:
                STextureStageCache()
                {
                    for (auto& i : current_texture_)
                    {
                        i = nullptr;
                    }
                }

                ~STextureStageCache()
                {
                    Clear();
                }

                void Set(u32 stage, const ITexture* tex)
                {
                    if (stage<MATERIAL_MAX_TEXTURES)
                    {
                        const ITexture* oldTexture = current_texture_[stage];
                        current_texture_[stage] = tex;
                        //delete oldTexture;
                    }
                }

                const ITexture* operator[](int stage) const
                {
                    if ((u32)stage<MATERIAL_MAX_TEXTURES)
                        return current_texture_[stage];
                    else
                        return nullptr;
                }

                void Remove(const ITexture* tex)
                {
                    for (s32 i = MATERIAL_MAX_TEXTURES - 1; i >= 0; --i)
                    {
                        if (current_texture_[i] == tex)
                        {
                            delete tex;
                            current_texture_[i] = nullptr;
                        }
                    }
                }

                void Clear()
                {
                    // Drop all the CurrentTexture handles
                    for (u32 i = 0; i<MATERIAL_MAX_TEXTURES; ++i)
                    {
                        if (current_texture_[i])
                        {
                            delete current_texture_[i];
                            current_texture_[i] = nullptr;
                        }
                    }
                }
            };
            STextureStageCache current_texture_;

#ifdef _KONG_WINDOWS_API_
            HDC hdc_; // Private GDI Device Context
            HWND window_;
            HGLRC hrc_;
#ifdef _KONG_COMPILE_WITH_WINDOWS_DEVICE_
            CKongDeviceWin32 *device_;
#endif
#endif
            SKongCreationParameters params_;
            core::Matrixf matrices_[ETS_COUNT];
            io::IFileSystem *io_;
            SMaterial material_;
            s32 max_texture_units_;
            s32 max_supported_textures_;
            core::Matrixf texture_flip_matrix_;
        };
    } // end namespace video
} // end namespace kong
#endif