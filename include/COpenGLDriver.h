// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _COPENGLDRIVER_H_
#define _COPENGLDRIVER_H_

#include "IVideoDriver.h"
#include "SMaterial.h"
#include <Windows.h>
#include "SKongCreationParameters.h"

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
            COpenGLDriver(const SKongCreationParameters &params,/* io::IFileSystem* io,*/ CKongDeviceWin32* device);

            bool InitDriver(CKongDeviceWin32 *device);

            //! Sets a material.
            /** All 3d drawing functions will draw geometry using this material thereafter.
            \param material: Material to be used from now on. */
            virtual void SetMaterial(const SMaterial& material);

            //! Applications must call this method before performing any rendering.
            /** This method can clear the back- and the z-buffer.
            \param backBuffer Specifies if the back buffer should be
            cleared, which means that the screen is filled with the color
            specified. If this parameter is false, the back buffer will
            not be cleared and the color parameter is ignored.
            \param zBuffer Specifies if the depth buffer (z buffer) should
            be cleared. It is not nesesarry to do so if only 2d drawing is
            used.
            \param color The color used for back buffer clearing
            \param videoData Handle of another window, if you want the
            bitmap to be displayed on another window. If this is an empty
            element, everything will be displayed in the default window.
            Note: This feature is not fully implemented for all devices.
            \param sourceRect Pointer to a rectangle defining the source
            rectangle of the area to be presented. Set to null to present
            everything. Note: not implemented in all devices.
            \return False if failed. */
            bool BeginScene(bool back_buffer = true, bool z_buffer = true, SColor color = SColor(255, 0, 0, 0)) override;

            //! Presents the rendered image to the screen.
            /** Applications must call this method after performing any
            rendering.
            \return False if failed and true if succeeded. */
            bool EndScene() override;

            //! Sets transformation matrices.
            /** \param state Transformation type to be set, e.g. view,
            world, or projection.
            \param mat Matrix describing the transformation. */
            virtual void SetTransform(E_TRANSFORMATION_STATE state, const core::Matrixf& mat);

        private:
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
        };
    } // end namespace video
} // end namespace kong
#endif