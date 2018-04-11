// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "COpenGLDriver.h"

#include "GL/gl.h"
//#include "GL/glew.h"
#include "IMeshBuffer.h"
#include "CImage.h"
#include "COpenGLTexture.h"
#include "IReadFile.h"

namespace kong
{
    namespace video
    {
        COpenGLDriver::COpenGLDriver(const SKongCreationParameters& params, io::IFileSystem* io, CKongDeviceWin32* device)
            : hdc_(nullptr), window_(static_cast<HWND>(params.window_id_)), hrc_(nullptr), device_(device), params_(params), io_(io)
        {
        }

        bool COpenGLDriver::InitDriver(CKongDeviceWin32* device)
        {
            // Set up pixel format descriptor with desired parameters
            PIXELFORMATDESCRIPTOR pfd = {
                sizeof(PIXELFORMATDESCRIPTOR),                  // Size Of This Pixel Format Descriptor
                1,                                              // Version Number
                PFD_DRAW_TO_WINDOW |                            // Format Must Support Window
                PFD_SUPPORT_OPENGL |                            // Format Must Support OpenGL
                PFD_DOUBLEBUFFER,                               // Format Must Support Double Buffering
                //(Params.Doublebuffer ? PFD_DOUBLEBUFFER : 0) |  // Must Support Double Buffering
                //(Params.Stereobuffer ? PFD_STEREO : 0),         // Must Support Stereo Buffer
                PFD_TYPE_RGBA,                                  // Request An RGBA Format
                params_.color_bits_,                            // Select Our Color Depth
                0, 0, 0, 0, 0, 0,                               // Color Bits Ignored
                0,                                              // No Alpha Buffer
                0,                                              // Shift Bit Ignored
                0,                                              // No Accumulation Buffer
                0, 0, 0, 0,	                                    // Accumulation Bits Ignored
                params_.z_buffer_bits_,                         // Z-Buffer (Depth Buffer)
                BYTE(params_.stencil_buffer_ ? 1 : 0),          // Stencil Buffer Depth
                0,                                              // No Auxiliary Buffer
                PFD_MAIN_PLANE,                                 // Main Drawing Layer
                0,                                              // Reserved
                0, 0, 0                                         // Layer Masks Ignored
            };

            GLuint pixel_format = 0;

            // windows created by device
            // get hdc
            hdc_ = GetDC(window_);
            if (!hdc_)
            {
                //os::Printer::log("Cannot create a GL device context.", ELL_ERROR);
                return false;
            }

            // search for pixel format the simple way
            if (pixel_format == 0 || (!SetPixelFormat(hdc_, pixel_format, &pfd)))
            {
                for (u32 i = 0; i < 5; ++i)
                {
                    if (i == 1)
                    {
                        if (params_.stencil_buffer_)
                        {
                            //os::Printer::log("Cannot create a GL device with stencil buffer, disabling stencil shadows.", ELL_WARNING);
                            params_.stencil_buffer_ = false;
                            pfd.cStencilBits = 0;
                        }
                        else
                            continue;
                    }
                    else if (i == 2)
                    {
                        pfd.cDepthBits = 24;
                    }
                    else if (i == 3)
                    {
                        if (params_.color_bits_ != 16)
                            pfd.cColorBits = 16;
                        else
                            continue;
                    }
                    else if (i == 4)
                    {
                        //os::Printer::log("Cannot create a GL device context", "No suitable format.", ELL_ERROR);
                        return false;
                    }

                    // choose pixelformat
                    pixel_format = ChoosePixelFormat(hdc_, &pfd);
                    if (pixel_format)
                        break;
                }

                // set pixel format
                if (!SetPixelFormat(hdc_, pixel_format, &pfd))
                {
                    //os::Printer::log("Cannot set the pixel format.", ELL_ERROR);
                    return false;
                }
            }

            hrc_ = wglCreateContext(hdc_);
            if (!hrc_)
            {
                //os::Printer::log("Cannot create a GL rendering context.", ELL_ERROR);
                return false;
            }
            wglMakeCurrent(hdc_, hrc_);

            return true;
        }

        void COpenGLDriver::SetMaterial(const SMaterial& material)
        {

        }

        bool COpenGLDriver::BeginScene(bool back_buffer, bool z_buffer, SColor color)
        {
            ClearBuffers(back_buffer, z_buffer, false, color);
            return true;
        }

        bool COpenGLDriver::EndScene()
        {
            glFlush();

#ifdef _KONG_COMPILE_WITH_WINDOWS_DEVICE_
            return SwapBuffers(hdc_) == TRUE;
#endif
        }

        void COpenGLDriver::SetTransform(E_TRANSFORMATION_STATE state, const core::Matrixf& mat)
        {
            matrices_[state] = mat;

            switch (state)
            {
            case ETS_VIEW:
            case ETS_WORLD:
                {
                    // OpenGL only has a model matrix, view and world is not existent. so lets fake these two.
                    glMatrixMode(GL_MODELVIEW);

                    // first load the viewing transformation for user clip planes
                    glLoadMatrixf((matrices_[ETS_VIEW]).Pointer());

                    //// we have to update the clip planes to the latest view matrix
                    //for (u32 i = 0; i < MaxUserClipPlanes; ++i)
                    //if (UserClipPlanes[i].Enabled)
                    //    uploadClipPlane(i);

                    // now the real model-view matrix
                    glMultMatrixf(matrices_[ETS_WORLD].Pointer());
                }
                break;
            case ETS_PROJECTION:
                {
                    glMatrixMode(GL_PROJECTION);
                    glLoadMatrixf(mat.Pointer());
                }
                break;
            case ETS_COUNT:
                return;
            }
        }

        void COpenGLDriver::Draw3DLine(const core::Vector3Df& start, const core::Vector3Df& end, SColor color)
        {
            glBegin(GL_LINES);
            glColor4ub(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
            glVertex3f(start.x_, start.y_, start.z_);

            glVertex3f(end.x_, end.y_, end.z_);
            glEnd();
        }

        void COpenGLDriver::DrawMeshBuffer(const scene::IMeshBuffer* mesh_buffer)
        {
            const S3DVertex *vertices = static_cast<const S3DVertex*>(mesh_buffer->GetVertices());
            u32 vertice_count = mesh_buffer->GetVertexCount();
            const u16 *indices = mesh_buffer->GetIndices();
            const GLsizei indices_count = mesh_buffer->GetIndexCount();

            glEnableClientState(GL_COLOR_ARRAY);
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glEnableClientState(GL_NORMAL_ARRAY);

            const GLint color_size = 4;

            glColorPointer(GL_BGRA_EXT, GL_UNSIGNED_BYTE, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].color_);

            glNormalPointer(GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].normal_);
            glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].texcoord_);
            glVertexPointer(3, GL_FLOAT, sizeof(S3DVertex), &(static_cast<const S3DVertex*>(vertices))[0].pos_);

            glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_SHORT, indices);

            glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }

        IImage* COpenGLDriver::CreateImage(ECOLOR_FORMAT format, const core::Dimension2d<u32>& size)
        {
            if (IImage::IsRenderTargetOnlyFormat(format))
            {
                //os::Printer::log("Could not create IImage, format only supported for render target textures.", ELL_WARNING);
                return nullptr;
            }

            return new CImage(format, size);
        }

        IImage* COpenGLDriver::CreateImage(ECOLOR_FORMAT format, IImage* imageToCopy)
        {
            //os::Printer::log("Deprecated method, please create an empty image instead and use copyTo().", ELL_WARNING);
            if (IImage::IsRenderTargetOnlyFormat(format))
            {
                //os::Printer::log("Could not create IImage, format only supported for render target textures.", ELL_WARNING);
                return nullptr;
            }

            CImage* tmp = new CImage(format, imageToCopy->GetDimension());
            imageToCopy->CopyTo(tmp);
            return tmp;
        }

        //! Creates a software image from a file.
        IImage* COpenGLDriver::CreateImageFromFile(const io::path& filename)
        {
            if (!filename.size())
                return 0;

            IImage* image = 0;
            io::IReadFile* file = io_->CreateAndOpenFile(filename);

            if (file)
            {
                image = CreateImageFromFile(file);
                delete file;
            }
            else
            {
                //os::Printer::log("Could not open file of image", filename, ELL_WARNING);
            }

            return image;
        }


        //! Creates a software image from a file.
        IImage* COpenGLDriver::CreateImageFromFile(io::IReadFile* file)
        {
            if (!file)
                return 0;

            IImage* image = 0;

            s32 i;

            // try to load file based on file extension
            for (i = SurfaceLoader.Size() - 1; i >= 0; --i)
            {
                if (SurfaceLoader[i]->IsALoadableFileExtension(file->GetFileName()))
                {
                    // reset file position which might have changed due to previous loadImage calls
                    file->Seek(0);
                    image = SurfaceLoader[i]->LoadImage(file);
                    if (image)
                        return image;
                }
            }

            // try to load file based on what is in it
            for (i = SurfaceLoader.Size() - 1; i >= 0; --i)
            {
                // dito
                file->Seek(0);
                if (SurfaceLoader[i]->IsALoadableFileFormat(file))
                {
                    file->Seek(0);
                    image = SurfaceLoader[i]->LoadImage(file);
                    if (image)
                        return image;
                }
            }

            return 0; // failed to load
        }

        void COpenGLDriver::ClearBuffers(bool back_buffer, bool z_buffer, bool stencil_buffer, SColor color)
        {
            GLbitfield mask = 0;

            if (back_buffer)
            {
                const f32 inv = 1.f / 255.f;
                glClearColor(color.GetRed() * inv, color.GetGreen() * inv, color.GetBlue() * inv, color.GetAlpha() * inv);
                mask |= GL_COLOR_BUFFER_BIT;
            }

            if (z_buffer)
            {
                glDepthMask(GL_TRUE);
                mask |= GL_DEPTH_BUFFER_BIT;
            }

            if (stencil_buffer)
                mask |= GL_STENCIL_BUFFER_BIT;

            if (mask)
                glClear(mask);

            glEnable(GL_DEPTH_TEST);
        }

        video::ITexture* COpenGLDriver::LoadTextureFromFile(io::IReadFile* file, const io::path& hashName)
        {
            ITexture* texture = 0;
            IImage* image = CreateImageFromFile(file);

            if (image)
            {
                // create texture from surface
                texture = createDeviceDependentTexture(image, hashName.size() ? hashName : file->GetFileName());
                //os::Printer::log("Loaded texture", file->getFileName());
                //image->drop();
                delete image;
            }

            return texture;
        }

        //! looks if the image is already loaded
        video::ITexture* COpenGLDriver::FindTexture(const io::path& filename)
        {
            SSurface s;
            SDummyTexture dummy(filename);
            s.Surface = &dummy;

            s32 index = textures_.BinarySearch(s);
            if (index != -1)
                return textures_[index].Surface;

            return 0;
        }

        //! loads a Texture
        ITexture* COpenGLDriver::GetTexture(const io::path& filename)
        {
            // Identify textures by their absolute filenames if possible.
            const io::path absolutePath = io_->GetAbsolutePath(filename);

            ITexture* texture = FindTexture(absolutePath);
            if (texture)
                return texture;

            // Then try the raw filename, which might be in an Archive
            texture = FindTexture(filename);
            if (texture)
                return texture;

            // Now try to open the file using the complete path.
            io::IReadFile* file = io_->CreateAndOpenFile(absolutePath);

            if (!file)
            {
                // Try to open it using the raw filename.
                file = io_->CreateAndOpenFile(filename);
            }

            if (file)
            {
                // Re-check name for actual archive names
                texture = FindTexture(file->GetFileName());
                if (texture)
                {
                    delete file;
                    return texture;
                }

                texture = LoadTextureFromFile(file);
                delete file;

                if (texture)
                {
                    AddTexture(texture);
                    //texture->drop(); // drop it because we created it, one grab too much
                }
                else
                {
                    //os::Printer::log("Could not load texture", filename, ELL_ERROR);
                }
                return texture;
            }
            else
            {
                //os::Printer::log("Could not open file of texture", filename, ELL_WARNING);
                return 0;
            }
        }


        //! loads a Texture
        ITexture* COpenGLDriver::GetTexture(io::IReadFile* file)
        {
            ITexture* texture = 0;

            if (file)
            {
                texture = FindTexture(file->GetFileName());

                if (texture)
                    return texture;

                texture = LoadTextureFromFile(file);

                if (texture)
                {
                    AddTexture(texture);
                    //texture->drop(); // drop it because we created it, one grab too much
                }

                if (!texture)
                {
                    //os::Printer::log("Could not load texture", file->getFileName(), ELL_WARNING);
                }
            }

            return texture;
        }

        //! Creates a texture from a loaded IImage.
        ITexture* COpenGLDriver::AddTexture(const io::path& name, IImage* image, void* mipmapData)
        {
            if (0 == name.size() || !image)
                return nullptr;

            ITexture* t = createDeviceDependentTexture(image, name, mipmapData);
            if (t)
            {
                AddTexture(t);
                //t->drop();
            }
            return t;
        }

        video::ITexture* COpenGLDriver::createDeviceDependentTexture(IImage* surface, const io::path& name, void* mipmapData)
        {
            return new COpenGLTexture(surface, name, mipmapData, this);
        }

        //! creates a Texture
        ITexture* COpenGLDriver::AddTexture(const core::Dimension2d<u32>& size,
            const io::path& name, ECOLOR_FORMAT format)
        {
            if (IImage::IsRenderTargetOnlyFormat(format))
            {
                //os::Printer::log("Could not create ITexture, format only supported for render target textures.", ELL_WARNING);
                return 0;
            }

            if (0 == name.size())
                return 0;

            IImage* image = new CImage(format, size);
            ITexture* t = createDeviceDependentTexture(image, name);
            //image->drop();
            delete image;
            AddTexture(t);

            //if (t)
            //    t->drop();

            return t;
        }

        //! adds a surface, not loaded or created by the Irrlicht Engine
        void COpenGLDriver::AddTexture(video::ITexture* texture)
        {
            if (texture)
            {
                SSurface s;
                s.Surface = texture;
                //texture->grab();

                textures_.PushBack(s);

                // the new texture is now at the end of the texture list. when searching for
                // the next new texture, the texture array will be sorted and the index of this texture
                // will be changed. to let the order be more consistent to the user, sort
                // the textures now already although this isn't necessary:

                textures_.Sort();
            }
        }
    } // end namespace video
} // end namespace kong

namespace kong
{
    namespace video
    {
#ifdef _KONG_COMPILE_WITH_OPENGL_
        IVideoDriver* CreateOpenGLDriver(const kong::SKongCreationParameters& params,
            io::IFileSystem* io, CKongDeviceWin32* device)
        {
            COpenGLDriver *driver = new COpenGLDriver(params, io, device);
#ifdef _KONG_COMPILE_WITH_OPENGL_
            if (!driver->InitDriver(device))
            {
                return nullptr;
            }
            return driver;
#else
            return nullptr;
#endif
        }
#endif
    }
}