// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "COpenGLDriver.h"

#include "GL/gl.h"
//#include "GL/glew.h"
#include "IMeshBuffer.h"
#include <cstdio>

namespace kong
{
    namespace video
    {
        COpenGLDriver::COpenGLDriver(const SKongCreationParameters& params, CKongDeviceWin32* device)
            : hdc_(nullptr), window_(static_cast<HWND>(params.window_id_)), hrc_(nullptr), device_(device), params_(params)
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

            //const void *pos_ptr = &(static_cast<const S3DVertex*>(vertices))[0].pos_;
            //printf("size : %d\n", sizeof(S3DVertex));
            //for (u32 i = 0; i < 24; i++)
            //{
            //    u32 off = sizeof(S3DVertex)* i / 4;
            //    printf("%f\t%f\t%f\n", (static_cast<const f32 *>(pos_ptr))[off + 0], static_cast<const f32 *>(pos_ptr)[off + 1],
            //        static_cast<const f32 *>(pos_ptr)[off + 2]);
            //}

            //printf("\n", sizeof(S3DVertex));
            //for (int i = 0; i < 24; i++)
            //{
            //    u32 off = sizeof(S3DVertex)* i;
            //    printf("%f\t%f\t%f\n", (static_cast<const S3DVertex*>(vertices))[i].pos_.x_, (static_cast<const S3DVertex*>(vertices))[i].pos_.y_,
            //        (static_cast<const S3DVertex*>(vertices))[i].pos_.z_);
            //}

            //glDrawArrays(GL_POINTS, 0, 24);
            glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_SHORT, indices);

            glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }

        void COpenGLDriver::ClearBuffers(bool back_buffer, bool z_buffer, bool stencil_buffer, SColor color)
        {
            GLbitfield mask = 0;

            if (back_buffer)
            {
                const f32 inv = 1.f / 255.f;
                glClearColor(color.GetRed() * inv, color.GetGreen() * inv, color.GetBlue() * inv, color.GetAlpha() * inv);
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
        }
    } // end namespace video
} // end namespace kong

namespace kong
{
    namespace video
    {
#ifdef _KONG_COMPILE_WITH_OPENGL_
        IVideoDriver* CreateOpenGLDriver(const kong::SKongCreationParameters& params,
            /*io::IFileSystem* io,*/ CKongDeviceWin32* device)
        {
            COpenGLDriver *driver = new COpenGLDriver(params, device);
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