// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _COPENGLDRIVER_H_
#define _COPENGLDRIVER_H_

#include "IVideoDriver.h"
#include "SMaterial.h"
#include <Windows.h>
#include "SKongCreationParameters.h"
#include "IFileSystem.h"


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

            //! Draws a mesh buffer
            void DrawMeshBuffer(const scene::IMeshBuffer* mesh_buffer) override;
        protected:
            //! enumeration for rendering modes such as 2d and 3d for minizing the switching of renderStates.
            enum E_RENDER_MODE
            {
                ERM_NONE = 0,	// no render state has been set yet.
                ERM_2D,		// 2d drawing rendermode
                ERM_3D		// 3d rendering mode
            };

            //! clears the zbuffer and color buffer
            void ClearBuffers(bool back_buffer, bool z_buffer, bool stencil_buffer, SColor color);

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
        };
    } // end namespace video
} // end namespace kong
#endif