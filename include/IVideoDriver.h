// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _I_VIDEO_DRIVER_H_
#define _I_VIDEO_DRIVER_H_
#include "SColor.h"
#include "Matrix.h"
#include "ITexture.h"

namespace kong
{
    namespace io
    {
        //class IAttributes;
        //struct SAttributeReadWriteOptions;
        class IReadFile;
        class IWriteFile;
    } // end namespace io

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
#if _KONG_MATERIAL_MAX_TEXTURES_>4
            //! Texture transformation
            ETS_TEXTURE_4,
#if _KONG_MATERIAL_MAX_TEXTURES_>5
            //! Texture transformation
            ETS_TEXTURE_5,
#if _KONG_MATERIAL_MAX_TEXTURES_>6
            //! Texture transformation
            ETS_TEXTURE_6,
#if _KONG_MATERIAL_MAX_TEXTURES_>7
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

            //! Creates an empty software image.
            /**
            \param format Desired color format of the image.
            \param size Size of the image to create.
            \return The created image.
            If you no longer need the image, you should call IImage::drop().
            See IReferenceCounted::drop() for more information. */
            virtual IImage* CreateImage(ECOLOR_FORMAT format, const core::Dimension2d<u32>& size) = 0;

            //! Creates a software image by converting it to given format from another image.
            /** \deprecated Create an empty image and use copyTo(). This method may be removed by Irrlicht 1.9.
            \param format Desired color format of the image.
            \param imageToCopy Image to copy to the new image.
            \return The created image.
            If you no longer need the image, you should call IImage::drop().
            See IReferenceCounted::drop() for more information. */
            _KONG_DEPRECATED_ virtual IImage* CreateImage(ECOLOR_FORMAT format, IImage *imageToCopy) = 0;

            //! Creates a software image from a file.
            /** No hardware texture will be created for this image. This
            method is useful for example if you want to read a heightmap
            for a terrain renderer.
            \param filename Name of the file from which the image is
            created.
            \return The created image.
            If you no longer need the image, you should call IImage::drop().
            See IReferenceCounted::drop() for more information. */
            virtual IImage* CreateImageFromFile(const io::path& filename) = 0;

            //! Creates a software image from a file.
            /** No hardware texture will be created for this image. This
            method is useful for example if you want to read a heightmap
            for a terrain renderer.
            \param file File from which the image is created.
            \return The created image.
            If you no longer need the image, you should call IImage::drop().
            See IReferenceCounted::drop() for more information. */
            virtual IImage* CreateImageFromFile(io::IReadFile* file) = 0;

            //! Get access to a named texture.
            /** Loads the texture from disk if it is not
            already loaded and generates mipmap levels if desired.
            Texture loading can be influenced using the
            setTextureCreationFlag() method. The texture can be in several
            imageformats, such as BMP, JPG, TGA, PCX, PNG, and PSD.
            \param filename Filename of the texture to be loaded.
            \return Pointer to the texture, or 0 if the texture
            could not be loaded. This pointer should not be dropped. See
            IReferenceCounted::drop() for more information. */
            virtual ITexture* GetTexture(const io::path& filename) = 0;

            //! Get access to a named texture.
            /** Loads the texture from disk if it is not
            already loaded and generates mipmap levels if desired.
            Texture loading can be influenced using the
            setTextureCreationFlag() method. The texture can be in several
            imageformats, such as BMP, JPG, TGA, PCX, PNG, and PSD.
            \param file Pointer to an already opened file.
            \return Pointer to the texture, or 0 if the texture
            could not be loaded. This pointer should not be dropped. See
            IReferenceCounted::drop() for more information. */
            virtual ITexture* GetTexture(io::IReadFile* file) = 0;

            //! Check if the image is already loaded.
            /** Works similar to getTexture(), but does not load the texture
            if it is not currently loaded.
            \param filename Name of the texture.
            \return Pointer to loaded texture, or 0 if not found. */
            virtual video::ITexture* FindTexture(const io::path& filename) = 0;

            //! Creates an empty texture of specified size.
            /** \param size: Size of the texture.
            \param name A name for the texture. Later calls to
            getTexture() with this name will return this texture
            \param format Desired color format of the texture. Please note
            that the driver may choose to create the texture in another
            color format.
            \return Pointer to the newly created texture. This pointer
            should not be dropped. See IReferenceCounted::drop() for more
            information. */
            virtual ITexture* AddTexture(const core::Dimension2d<u32>& size,
                const io::path& name, ECOLOR_FORMAT format = ECF_A8R8G8B8) = 0;

            //! Creates a texture from an IImage.
            /** \param name A name for the texture. Later calls of
            getTexture() with this name will return this texture
            \param image Image the texture is created from.
            \param mipmapData Optional pointer to a set of images which
            build up the whole mipmap set. Must be images of the same color
            type as image. If this parameter is not given, the mipmaps are
            derived from image.
            \return Pointer to the newly created texture. This pointer
            should not be dropped. See IReferenceCounted::drop() for more
            information. */
            virtual ITexture* AddTexture(const io::path& name, IImage* image, void* mipmapData = nullptr) = 0;
        };
    }
}

#endif