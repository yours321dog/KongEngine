// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _I_VIDEO_DRIVER_H_
#define _I_VIDEO_DRIVER_H_
#include "SColor.h"
#include "Matrix.h"

namespace kong
{
    namespace video
    {
        //! enumeration for geometry transformation states
        enum E_TRANSFORMATION_STATE
        {
            //! View transformation
            ETS_VIEW = 0,
            //! World transformation
            ETS_WORLD,
            //! Projection transformation
            ETS_PROJECTION,
            //! Texture transformation
            ETS_TEXTURE_0,
            //! Texture transformation
            ETS_TEXTURE_1,
            //! Texture transformation
            ETS_TEXTURE_2,
            //! Texture transformation
            ETS_TEXTURE_3,
#if _IRR_MATERIAL_MAX_TEXTURES_>4
            //! Texture transformation
            ETS_TEXTURE_4,
#if _IRR_MATERIAL_MAX_TEXTURES_>5
            //! Texture transformation
            ETS_TEXTURE_5,
#if _IRR_MATERIAL_MAX_TEXTURES_>6
            //! Texture transformation
            ETS_TEXTURE_6,
#if _IRR_MATERIAL_MAX_TEXTURES_>7
            //! Texture transformation
            ETS_TEXTURE_7,
#endif
#endif
#endif
#endif
            //! Not used
            ETS_COUNT
        };

        class IVideoDriver
        {
        public:
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
            virtual bool beginScene(bool back_buffer = true, bool z_buffer = true, SColor color = SColor(255, 0, 0, 0)) = 0;

            //! Presents the rendered image to the screen.
            /** Applications must call this method after performing any
            rendering.
            \return False if failed and true if succeeded. */
            virtual bool endScene() = 0;

            //! Sets transformation matrices.
            /** \param state Transformation type to be set, e.g. view,
            world, or projection.
            \param mat Matrix describing the transformation. */
            virtual void setTransform(E_TRANSFORMATION_STATE state, const core::Matrixf& mat) = 0;
        };
    }
}

#endif