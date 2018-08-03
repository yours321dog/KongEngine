// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _I_VIDEO_DRIVER_H_
#define _I_VIDEO_DRIVER_H_
#include "SColor.h"
#include "Matrix.h"
#include "ITexture.h"
#include "SLight.h"
#include "ERenderingMode.h"

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
        class IMeshManipulator;
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
            //! Light view transform
            ETS_LIGHT_VIEW,
            //! Light projection trasform
            ETS_LIGHT_PROJECTION,
            
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
            virtual void SetTransform(u32 state, const core::Matrixf& mat) = 0;

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

            //! Draws a 2d image using a color
            /** (if color is other than
            Color(255,255,255,255)) and the alpha channel of the texture.
            \param texture Texture to be drawn.
            \param destPos Upper left 2d destination position where the
            image will be drawn.
            \param sourceRect Source rectangle in the image.
            \param clipRect Pointer to rectangle on the screen where the
            image is clipped to.
            If this pointer is NULL the image is not clipped.
            \param color Color with which the image is drawn. If the color
            equals Color(255,255,255,255) it is ignored. Note that the
            alpha component is used: If alpha is other than 255, the image
            will be transparent.
            \param useAlphaChannelOfTexture: If true, the alpha channel of
            the texture is used to draw the image.*/
            virtual void Draw2DImage(const video::ITexture* texture, const core::position2d<s32>& destPos,
                const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
                SColor color = SColor(255, 255, 255, 255), bool useAlphaChannelOfTexture = false) = 0;

            //! Draws a 2d image using a color
            /** (if color is other than
            Color(255,255,255,255)) and the alpha channel of the texture.
            \param texture Texture to be drawn.
            \param destPos Upper left 2d destination position where the
            image will be drawn.
            \param sourceRect Source rectangle in the image.
            \param clipRect Pointer to rectangle on the screen where the
            image is clipped to.
            If this pointer is NULL the image is not clipped.
            \param color Color with which the image is drawn. If the color
            equals Color(255,255,255,255) it is ignored. Note that the
            alpha component is used: If alpha is other than 255, the image
            will be transparent.
            \param useAlphaChannelOfTexture: If true, the alpha channel of
            the texture is used to draw the image.*/
            virtual void Draw2DImage(const video::ITexture* texture, const core::position2d<f32>& destPos,
                const core::rect<f32>& sourceRect, SColor color = SColor(255, 255, 255, 255)) = 0;

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

            //! Returns a pointer to the mesh manipulator.
            virtual scene::IMeshManipulator* GetMeshManipulator() = 0;

            //! Creates a normal map from a height map texture.
            /** If the target texture has 32 bit, the height value is
            stored in the alpha component of the texture as addition. This
            value is used by the video::EMT_PARALLAX_MAP_SOLID material and
            similar materials.
            \param texture Texture whose alpha channel is modified.
            \param amplitude Constant value by which the height
            information is multiplied.*/
            virtual void MakeNormalMapTexture(video::ITexture* texture, f32 amplitude = 1.0f) const = 0;

            //! Get the size of the screen or render window.
            /** \return Size of screen or render window. */
            virtual const core::Dimension2d<u32>& GetScreenSize() const = 0;

            //! Get the size of the current render target
            /** This method will return the screen size if the driver
            doesn't support render to texture, or if the current render
            target is the screen.
            \return Size of render target or screen/window */
            virtual const core::Dimension2d<u32>& GetCurrentRenderTargetSize() const = 0;

            //! Deletes all dynamic lights which were previously added with addDynamicLight().
            virtual void DeleteAllDynamicLights() = 0;

            //! adds a dynamic light, returning an index to the light
            //! \param light: the light data to use to create the light
            //! \return An index to the light, or -1 if an error occurs
            virtual s32 AddDynamicLight(const SLight& light) = 0;

            //! activate dynamic lights, returning an index to the light
            virtual void ActivateDynamicLights() = 0;

            //! Set main light, used for shadow rendering
            virtual void SetMainLight(const SLight& light) = 0;

            //! Returns the maximal amount of dynamic lights the device can handle
            /** \return Maximal amount of dynamic lights. */
            virtual u32 GetMaximalDynamicLightAmount() const = 0;

            //! Returns amount of dynamic lights currently set
            /** \return Amount of dynamic lights currently set */
            virtual u32 GetDynamicLightCount() const = 0;

            //! Returns light data which was previously set by IVideoDriver::addDynamicLight().
            /** \param idx Zero based index of the light. Must be 0 or
            greater and smaller than IVideoDriver::getDynamicLightCount.
            \return Light data. */
            virtual const SLight& GetDynamicLight(u32 idx) const = 0;

            //! Set active camera position
            virtual void SetActiveCameraPosition(core::Vector3Df position) const = 0;

            //! Set rendering mode
            /** This method will change the rendering mode.
             * \param mode switch between wireframe and mesh rendering mode */
            virtual void SetRenderingMode(E_RENDERING_MODE mode = ERM_MESH) = 0;

            //! Get the current color format of the color buffer
            /** \return Color format of the color buffer. */
            virtual ECOLOR_FORMAT GetColorFormat() const = 0;

            //! Enable shadows.
            virtual void EnableShadow(bool flag = true) = 0;

            //! Begin shadow rendering
            virtual void BeginShadowRender() = 0;

            //! End shadow rendering
            virtual void EndShadowRender() = 0;

            //! Sets a new viewport.
            /** Every rendering operation is done into this new area.
            \param area: Rectangle defining the new area of rendering
            operations. */
            virtual void setViewPort(const core::rect<s32>& area) = 0;

            //! Gets the area of the current viewport.
            /** \return Rectangle of the current viewport. */
            virtual const core::rect<s32>& getViewPort() const = 0;

            //! Render first pass
            /** if you want to use deferred rendering , you need to use
             *  this method to run the first pass. */
            virtual void RenderFirstPass() = 0;

            //! Render second pass
            /** if you want to use deferred rendering , you need to use
            *  this method to run the second pass. */
            virtual void RenderSecondPass() = 0;

            virtual void CheckError() = 0;

            // draw a space fill quad
            virtual void DrawSpaceFillQuad() = 0;
        };
    }
}

#endif