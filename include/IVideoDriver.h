// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _I_VIDEO_DRIVER_H_
#define _I_VIDEO_DRIVER_H_
#include "SColor.h"
#include "Matrix.h"

namespace kong
{
    namespace scene
    {
        class IMeshBuffer;
        class IMesh;
        //class IMeshManipulator;
        class ISceneNode;
    } // end namespace scene


    namespace video
    {
        class SMaterial;

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
            virtual ~IVideoDriver() = default;

            //! Sets a material.
            /** All 3d drawing functions will draw geometry using this material thereafter.
            \param material: Material to be used from now on. */
            virtual void SetMaterial(const SMaterial& material) = 0;

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
            virtual bool BeginScene(bool back_buffer = true, bool z_buffer = true, SColor color = SColor(255, 0, 0, 0)) = 0;

            //! Presents the rendered image to the screen.
            /** Applications must call this method after performing any
            rendering.
            \return False if failed and true if succeeded. */
            virtual bool EndScene() = 0;

            //! Sets transformation matrices.
            /** \param state Transformation type to be set, e.g. view,
            world, or projection.
            \param mat Matrix describing the transformation. */
            virtual void SetTransform(E_TRANSFORMATION_STATE state, const core::Matrixf& mat) = 0;

            //! Draws a 3d line.
            /** For some implementations, this method simply calls
            drawVertexPrimitiveList for some triangles.
            Note that the line is drawn using the current transformation
            matrix and material. So if you need to draw the 3D line
            independently of the current transformation, use
            \code
            driver->setMaterial(someMaterial);
            driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);
            \endcode
            for some properly set up material before drawing the line.
            Some drivers support line thickness set in the material.
            \param start Start of the 3d line.
            \param end End of the 3d line.
            \param color Color of the line. */
            virtual void Draw3DLine(const core::Vector3Df& start,
                const core::Vector3Df& end, SColor color = SColor(255, 255, 255, 255)) = 0;

            //! Draws a mesh buffer
            /** \param mb Buffer to draw */
            virtual void DrawMeshBuffer(const scene::IMeshBuffer* mb) = 0;
        };
    }
}

#endif