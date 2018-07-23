// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "COpenGLDriver.h"

#include <GL/glew.h>
#include "IMeshBuffer.h"
#include "CImage.h"
#include "COpenGLTexture.h"
#include "IReadFile.h"
#include "CMeshManipulator.h"
#include "os.h"

namespace kong
{
    namespace video
    {
        //! creates a loader which is able to load png images
        IImageLoader* CreateImageLoaderPng();

        //! creates a loader which is able to load jpeg images
        IImageLoader* CreateImageLoaderJpg();

        COpenGLDriver::COpenGLDriver(const SKongCreationParameters& params, io::IFileSystem* io, CKongDeviceWin32* device)
            : hdc_(nullptr), window_(static_cast<HWND>(params.window_id_)), hrc_(nullptr), device_(device), params_(params),
              io_(io), max_texture_units_(0), max_supported_textures_(0), max_support_lights_(0)
        {
            // create manipulator
            mesh_manipulator_ = new scene::CMeshManipulator();


#ifdef _KONG_COMPILE_WITH_JPG_LOADER_
            surface_loader_.PushBack(video::CreateImageLoaderJpg());
#endif
#ifdef _KONG_COMPILE_WITH_PNG_LOADER_
            surface_loader_.PushBack(video::CreateImageLoaderPng());
#endif
        }

        COpenGLDriver::~COpenGLDriver()
        {
            delete mesh_manipulator_;
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
            if (glewInit() != GLEW_OK)
            {
                printf("Glew init error\n");
                return false;
            }

            glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units_);
            max_texture_units_ = core::min_<u32>(max_texture_units_, MATERIAL_MAX_TEXTURES);
            max_supported_textures_ = max_texture_units_;
            UpdateMaxSupportLights();

            return true;
        }

        void COpenGLDriver::SetMaterial(const SMaterial& material)
        {
            material_ = material;

            for (auto i = max_texture_units_ - 1; i >= 0; --i)
            {
                SetActiveTexture(i, material.GetTexture(i));
                const u32 texture_val = ETS_TEXTURE_0 + 1;
                SetTransform(texture_val, material_.GetTextureMatrix(i));
            }
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

        void COpenGLDriver::SetTransform(u32 state, const core::Matrixf& mat)
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
            default:
            {
                const u32 i = state - ETS_TEXTURE_0;
                if (i >= MATERIAL_MAX_TEXTURES)
                    break;

                const bool isRTT = material_.GetTexture(i) && material_.GetTexture(i)->IsRenderTarget();

                //if (MultiTextureExtension)
                //    extGlActiveTexture(GL_TEXTURE0_ARB + i);

                glMatrixMode(GL_TEXTURE);
                if (!isRTT && mat.IsIdentity())
                    glLoadIdentity();
                else
                {
                    GLfloat glmat[16];
                    if (isRTT)
                        GetGLTextureMatrix(glmat, mat * texture_flip_matrix_);
                    else
                        GetGLTextureMatrix(glmat, mat);
                    glLoadMatrixf(glmat);
                }
                break;
            }
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
            for (i = surface_loader_.Size() - 1; i >= 0; --i)
            {
                if (surface_loader_[i]->IsALoadableFileExtension(file->GetFileName()))
                {
                    // reset file position which might have changed due to previous loadImage calls
                    file->Seek(0);
                    image = surface_loader_[i]->LoadImage(file);
                    if (image)
                        return image;
                }
            }

            // try to load file based on what is in it
            for (i = surface_loader_.Size() - 1; i >= 0; --i)
            {
                // dito
                file->Seek(0);
                if (surface_loader_[i]->IsALoadableFileFormat(file))
                {
                    file->Seek(0);
                    image = surface_loader_[i]->LoadImage(file);
                    if (image)
                        return image;
                }
            }

            return nullptr; // failed to load
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

            // Enable blending
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

        //! creates a matrix in supplied GLfloat array to pass to OpenGL
        inline void COpenGLDriver::GetGLMatrix(f32 gl_matrix[16], const core::Matrixf& m)
        {
            memcpy(gl_matrix, m.Pointer(), 16 * sizeof(f32));
        }


        //! creates a opengltexturematrix from a D3D style texture matrix
        inline void COpenGLDriver::GetGLTextureMatrix(f32 *o, const core::Matrixf& m)
        {
            o[0] = m[0];
            o[1] = m[1];
            o[2] = 0.f;
            o[3] = 0.f;

            o[4] = m[4];
            o[5] = m[5];
            o[6] = 0.f;
            o[7] = 0.f;

            o[8] = 0.f;
            o[9] = 0.f;
            o[10] = 1.f;
            o[11] = 0.f;

            o[12] = m[8];
            o[13] = m[9];
            o[14] = 0.f;
            o[15] = 1.f;
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

            return nullptr;
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
                return nullptr;
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

            if (name.size() == 0)
            {
                return nullptr;
            }

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

        bool COpenGLDriver::SetActiveTexture(u32 stage, const video::ITexture* texture)
        {
            if (current_texture_[stage] == texture)
                return true;

            current_texture_.Set(stage, texture);

            if (!texture)
            {
                glDisable(GL_TEXTURE_2D);
                return true;
            }
            else
            {
                if (texture->GetDriverType() != EDT_OPENGL)
                {
                    glDisable(GL_TEXTURE_2D);
                    current_texture_.Set(stage, nullptr);
                    //os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
                    return false;
                }

                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, dynamic_cast<const COpenGLTexture*>(texture)->GetOpenGLTextureName());
            }
            return true;
        }

        bool COpenGLDriver::DisableTextures(u32 fromStage)
        {
            bool result = true;
            for (u32 i = fromStage; i<max_supported_textures_; ++i)
                result &= SetActiveTexture(i, nullptr);
            return result;
        }

        scene::IMeshManipulator* COpenGLDriver::GetMeshManipulator()
        {
            return mesh_manipulator_;
        }

        void COpenGLDriver::MakeNormalMapTexture(video::ITexture* texture, f32 amplitude) const
        {
            if (!texture)
                return;

            if (texture->GetColorFormat() != ECF_A1R5G5B5 &&
                texture->GetColorFormat() != ECF_A8R8G8B8)
            {
                os::Printer::log("Error: Unsupported texture color format for making normal map.", ELL_ERROR);
                return;
            }

            core::Dimension2d<u32> dim = texture->GetSize();
            amplitude = amplitude / 255.0f;
            f32 vh = dim.height_ / (f32)dim.width_;
            f32 hh = dim.height_ / (f32)dim.height_;

            if (texture->GetColorFormat() == ECF_A8R8G8B8)
            {
                // ECF_A8R8G8B8 version

                s32 *p = static_cast<s32*>(texture->Lock());

                if (!p)
                {
                    os::Printer::log("Could not lock texture for making normal map.", ELL_ERROR);
                    return;
                }

                // copy texture

                u32 pitch = texture->GetPitch() / 4;

                s32* in = new s32[dim.height_ * pitch];
                memcpy(in, p, dim.height_ * pitch * 4);

                for (s32 x = 0; x < s32(pitch); ++x)
                    for (s32 y = 0; y < s32(dim.height_); ++y)
                    {
                        // TODO: this could be optimized really a lot

                        core::vector3df h1((x - 1)*hh, nml32(x - 1, y, pitch, dim.height_, in)*amplitude, y*vh);
                        core::vector3df h2((x + 1)*hh, nml32(x + 1, y, pitch, dim.height_, in)*amplitude, y*vh);
                        //core::vector3df v1(x*hh, nml32(x, y-1, pitch, dim.Height, in)*amplitude, (y-1)*vh);
                        //core::vector3df v2(x*hh, nml32(x, y+1, pitch, dim.Height, in)*amplitude, (y+1)*vh);
                        core::vector3df v1(x*hh, nml32(x, y + 1, pitch, dim.height_, in)*amplitude, (y - 1)*vh);
                        core::vector3df v2(x*hh, nml32(x, y - 1, pitch, dim.height_, in)*amplitude, (y + 1)*vh);

                        core::vector3df v = v1 - v2;
                        core::vector3df h = h1 - h2;

                        core::vector3df n = v.CrossProduct(h);
                        n.Normalize();
                        n *= 0.5f;
                        n += core::vector3df(0.5f, 0.5f, 0.5f); // now between 0 and 1
                        n *= 255.0f;

                        s32 height = (s32)nml32(x, y, pitch, dim.height_, in);
                        p[y*pitch + x] = video::SColor(
                            height, // store height in alpha
                            (s32)n.x_, (s32)n.z_, (s32)n.y_).color_;
                    }

                delete[] in;
                texture->Unlock();
            }
            else
            {
                // ECF_A1R5G5B5 version

                s16 *p = (s16*)texture->Lock();

                if (!p)
                {
                    os::Printer::log("Could not lock texture for making normal map.", ELL_ERROR);
                    return;
                }

                u32 pitch = texture->GetPitch() / 2;

                // copy texture

                s16* in = new s16[dim.height_ * pitch];
                memcpy(in, p, dim.height_ * pitch * 2);

                for (s32 x = 0; x < s32(pitch); ++x)
                    for (s32 y = 0; y < s32(dim.height_); ++y)
                    {
                        // TODO: this could be optimized really a lot

                        core::vector3df h1((x - 1)*hh, nml16(x - 1, y, pitch, dim.height_, in)*amplitude, y*vh);
                        core::vector3df h2((x + 1)*hh, nml16(x + 1, y, pitch, dim.height_, in)*amplitude, y*vh);
                        core::vector3df v1(x*hh, nml16(x, y - 1, pitch, dim.height_, in)*amplitude, (y - 1)*vh);
                        core::vector3df v2(x*hh, nml16(x, y + 1, pitch, dim.height_, in)*amplitude, (y + 1)*vh);

                        core::vector3df v = v1 - v2;
                        core::vector3df h = h1 - h2;

                        core::vector3df n = v.CrossProduct(h);
                        n.Normalize();
                        n *= 0.5f;
                        n += core::vector3df(0.5f, 0.5f, 0.5f); // now between 0 and 1
                        n *= 255.0f;

                        p[y*pitch + x] = video::RGBA16((u32)n.x_, (u32)n.z_, (u32)n.y_);
                    }

                delete[] in;
                texture->Unlock();
            }

            texture->RegenerateMipMapLevels();
        }

        const core::Dimension2d<u32>& COpenGLDriver::GetScreenSize() const
        {
            return params_.window_size_;
        }

        const core::Dimension2d<u32>& COpenGLDriver::GetCurrentRenderTargetSize() const
        {
            return params_.window_size_;
        }

        void COpenGLDriver::DeleteAllDynamicLights()
        {
            lights_.Resize(0);
        }

        s32 COpenGLDriver::AddDynamicLight(const SLight& light)
        {
            lights_.PushBack(light);

            return lights_.Size() - 1;
        }

        u32 COpenGLDriver::GetMaximalDynamicLightAmount() const
        {
            return max_support_lights_;
        }

        u32 COpenGLDriver::GetDynamicLightCount() const
        {
            return lights_.Size();
        }

        const SLight& COpenGLDriver::GetDynamicLight(u32 idx) const
        {
            if (idx < lights_.Size())
                return lights_[idx];
            else
                return *static_cast<SLight*>(nullptr);
        }

        void COpenGLDriver::SetActiveCameraPosition(core::Vector3Df position) const
        {
        }

        void COpenGLDriver::SetRenderingMode(E_RENDERING_MODE mode)
        {
            rendering_mode_ = mode;
            switch (mode)
            {
            default:
            case ERM_MESH:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
            case ERM_WIREFRAME:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
            }
        }

        void COpenGLDriver::UpdateMaxSupportLights()
        {
            max_support_lights_ = 8;
        }

        //! draws a 2d image, using a color and the alpha channel of the texture if
        //! desired. The image is drawn at pos, clipped against clipRect (if != 0).
        //! Only the subtexture defined by sourceRect is used.
        void COpenGLDriver::Draw2DImage(const video::ITexture* texture,
            const core::position2d<s32>& pos,
            const core::rect<s32>& sourceRect,
            const core::rect<s32>* clipRect, SColor color,
            bool useAlphaChannelOfTexture)
        {
            //if (!texture)
            //    return;

            //if (!sourceRect.isValid())
            //    return;

            //DisableTextures(1);
            //if (!SetActiveTexture(0, texture))
            //    return;
            ////setRenderStates2DMode(color.GetAlpha()<255, true, useAlphaChannelOfTexture);

            //core::rect<f32> 

            //glColor4ub(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
            //glBegin(GL_QUADS);

            //glTexCoord2f(tcoords.UpperLeftCorner.x_, tcoords.UpperLeftCorner.y_);
            //glVertex2f(GLfloat(poss.UpperLeftCorner.x_), GLfloat(poss.UpperLeftCorner.y_));

            //glTexCoord2f(tcoords.LowerRightCorner.x_, tcoords.UpperLeftCorner.y_);
            //glVertex2f(GLfloat(poss.LowerRightCorner.x_), GLfloat(poss.UpperLeftCorner.y_));

            //glTexCoord2f(tcoords.LowerRightCorner.x_, tcoords.LowerRightCorner.y_);
            //glVertex2f(GLfloat(poss.LowerRightCorner.x_), GLfloat(poss.LowerRightCorner.y_));

            //glTexCoord2f(tcoords.UpperLeftCorner.x_, tcoords.LowerRightCorner.y_);
            //glVertex2f(GLfloat(poss.UpperLeftCorner.x_), GLfloat(poss.LowerRightCorner.y_));

            //glEnd();
        }

        void COpenGLDriver::Draw2DImage(const video::ITexture* texture, const core::Vector<f32>& destPos, const core::rect<f32>& sourceRect, SColor color)
        {
            if (!texture)
                return;

            if (!sourceRect.isValid())
                return;

            core::rect<f32> poss = sourceRect + destPos;
            core::rect<f32> tcoords(0.f, 1.f, 1.f, 0.f);

            DisableTextures(1);
            if (!SetActiveTexture(0, texture))
            {
                return;
            }

            glColor4ub(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
            glBegin(GL_QUADS);

            glTexCoord2f(tcoords.UpperLeftCorner.x_, tcoords.UpperLeftCorner.y_);
            glVertex2f(GLfloat(poss.UpperLeftCorner.x_), GLfloat(poss.UpperLeftCorner.y_));

            glTexCoord2f(tcoords.LowerRightCorner.x_, tcoords.UpperLeftCorner.y_);
            glVertex2f(GLfloat(poss.LowerRightCorner.x_), GLfloat(poss.UpperLeftCorner.y_));

            glTexCoord2f(tcoords.LowerRightCorner.x_, tcoords.LowerRightCorner.y_);
            glVertex2f(GLfloat(poss.LowerRightCorner.x_), GLfloat(poss.LowerRightCorner.y_));

            glTexCoord2f(tcoords.UpperLeftCorner.x_, tcoords.LowerRightCorner.y_);
            glVertex2f(GLfloat(poss.UpperLeftCorner.x_), GLfloat(poss.LowerRightCorner.y_));

            glEnd();
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
            //COpenGLDriver *driver = nullptr;
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