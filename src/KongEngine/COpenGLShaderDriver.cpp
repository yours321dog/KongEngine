// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "COpenGLShaderDriver.h"
#include "GL/glew.h"
#include "S3DVertex.h"
#include "IMeshBuffer.h"
#include "COpenGLTexture.h"
#include "COpenGLShaderHelper.h"
#include "os.h"

namespace kong
{
    namespace video
    {

        COpenGLShaderDriver::COpenGLShaderDriver(const SKongCreationParameters& params, io::IFileSystem* file_system, CKongDeviceWin32* device,
            io::SPath vertex_path, io::SPath fragment_path)
            : COpenGLDriver(params, file_system, device), shader_helper_(nullptr), shadow_shader_helper_(nullptr),
              base_shader_helper_(nullptr), vao_(0), vbo_(0), ebo_(0),
              vertex_path_(vertex_path), fragment_path_(fragment_path)
        {
        }

        COpenGLShaderDriver::~COpenGLShaderDriver()
        {
            delete shadow_shader_helper_;
            delete base_shader_helper_;
        }

        bool COpenGLShaderDriver::InitDriver(CKongDeviceWin32* device)
        {
            if (!COpenGLDriver::InitDriver(device))
            {
                return false;
            }

            shader_helper_ = new COpenGLShaderHelper(io_, vertex_path_, fragment_path_);
            //shader_helper_ = new COpenGLShaderHelper(io_, io::SPath("./shaders/shadow.vs"), io::SPath("./shaders/shadow.fs"));
            base_shader_helper_ = shader_helper_;
            shadow_shader_helper_ = new COpenGLShaderHelper(io_, io::SPath("./shaders/shadow.vs"), io::SPath("./shaders/shadow.fs"));

            glGenVertexArrays(1, &vao_);
            glGenBuffers(1, &vbo_);
            glGenBuffers(1, &ebo_);

            if (shader_helper_ == nullptr)
            {
                return false;
            }

            shadow_shader_helper_->Use();
            shadow_shader_helper_->SetBool("test_on", false);

            shader_helper_->Use();
            shader_helper_->SetBool("texture0_on", false);
            shader_helper_->SetBool("texture1_on", false);
            shader_helper_->SetBool("normal_mapping_on", false);
            shader_helper_->SetBool("light_on", false);
            shader_helper_->SetBool("light0_on", false);

            return true;
        }

        void COpenGLShaderDriver::SetMaterial(const SMaterial& material)
        {
#ifdef _DEBUG
            CheckError();
#endif
            COpenGLDriver::SetMaterial(material);
#ifdef _DEBUG
            CheckError();
#endif
            GLfloat data[4];
            data[0] = material.ambient_color_.GetRed() / 255.f;
            data[1] = material.ambient_color_.GetGreen() / 255.f;
            data[2] = material.ambient_color_.GetBlue() / 255.f;
            data[3] = material.ambient_color_.GetAlpha() / 255.f;
            SetMaterialUniform(SL_MAT_AMBIENT, data);
#ifdef _DEBUG
            CheckError();
#endif

            data[0] = material.diffuse_color_.GetRed() / 255.f;
            data[1] = material.diffuse_color_.GetGreen() / 255.f;
            data[2] = material.diffuse_color_.GetBlue() / 255.f;
            data[3] = material.diffuse_color_.GetAlpha() / 255.f;
            SetMaterialUniform(SL_MAT_DIFFUSE, data);

            data[0] = material.specular_color_.GetRed() / 255.f;
            data[1] = material.specular_color_.GetGreen() / 255.f;
            data[2] = material.specular_color_.GetBlue() / 255.f;
            data[3] = material.specular_color_.GetAlpha() / 255.f;
            SetMaterialUniform(SL_MAT_SPECULAR, data);

            data[0] = material.emissive_color_.GetRed() / 255.f;
            data[1] = material.emissive_color_.GetGreen() / 255.f;
            data[2] = material.emissive_color_.GetBlue() / 255.f;
            data[3] = material.emissive_color_.GetAlpha() / 255.f;
            SetMaterialUniform(SL_MAT_EMISSIVE, data);

#ifdef _DEBUG
            CheckError();
#endif

            SetMaterialUniform(SL_MAT_SHININESS, material.shininess_);

#ifdef _DEBUG
            CheckError();
#endif

            if (material.MaterialType == EMT_NORMAL_MAP_SOLID || material.MaterialType == EMT_PARALLAX_MAP_SOLID
                || material.MaterialType == EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR || material.MaterialType == EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR)
            {
                shader_helper_->SetBool("normal_mapping_on", true);
            }
            else
            {
                shader_helper_->SetBool("normal_mapping_on", false);
            }
#ifdef _DEBUG
            CheckError();
#endif
        }

        void COpenGLShaderDriver::SetTransform(u32 state, const core::Matrixf& mat)
        {
            matrices_[state] = mat;

            switch (state)
            {
            case ETS_VIEW:
                shader_helper_->SetMatrix4(std::string("view_transform"), mat);
                break;
            case ETS_WORLD:
                //// we have to update the clip planes to the latest view matrix
                //for (u32 i = 0; i < MaxUserClipPlanes; ++i)
                //if (UserClipPlanes[i].Enabled)
                //    uploadClipPlane(i);

                // now the real model-view matrix
                shader_helper_->SetMatrix4(std::string("world_transform"), mat);
                break;
            case ETS_PROJECTION:
                shader_helper_->SetMatrix4(std::string("project_transform"), mat);
                break;
            case ETS_LIGHT_VIEW:
                shader_helper_->SetMatrix4(std::string("light_view_transform"), mat);
                break;
            case ETS_LIGHT_PROJECTION:
                shader_helper_->SetMatrix4(std::string("light_projection_transform"), mat);
                break;
            case ETS_COUNT:
            default:
                return;
            }
        }

        void COpenGLShaderDriver::DrawMeshBuffer(const scene::IMeshBuffer* mesh_buffer)
        {
            if (material_.MaterialType == EMT_NORMAL_MAP_SOLID || material_.MaterialType == EMT_PARALLAX_MAP_SOLID
                || material_.MaterialType == EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR || material_.MaterialType == EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR)
            {
                DrawTangentMeshBuffer(mesh_buffer);
            }
            else
            {
                DrawNormalMeshBuffer(mesh_buffer);
            }
        }

        bool COpenGLShaderDriver::SetActiveTexture(u32 stage, const video::ITexture* texture)
        {
            if (!render_material_texture_on_)
                return false;

            if (current_texture_[stage] == texture)
                return true;

            current_texture_.Set(stage, texture);

            core::stringc str_on = core::stringc("texture") + core::stringc(stage) + "_on";
            
            shader_helper_->Use();
            if (texture == nullptr)
            {
                Disable(SL_TEXTURE0 + stage);
                return false;
            }
            else
            {
                if (texture->GetDriverType() != EDT_OPENGL)
                {
                    Disable(SL_TEXTURE0 + stage);
                    current_texture_.Set(stage, nullptr);
                    //os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
                    return false;
                }

                shader_helper_->SetInt(GetUniformName(SL_TEXTURE0 + stage), stage);
                glActiveTexture(GL_TEXTURE0 + stage);
                glBindTexture(GL_TEXTURE_2D, dynamic_cast<const COpenGLTexture*>(texture)->GetOpenGLTextureName());
                shader_helper_->SetBool(str_on.c_str(), true);
            }
            return true;
        }

        void COpenGLShaderDriver::DeleteAllDynamicLights()
        {
            for (auto i = 0; i < max_support_lights_; i++)
            {
                Disable(SL_LIGHT0 + i);
            }

            COpenGLDriver::DeleteAllDynamicLights();
        }

        s32 COpenGLShaderDriver::AddDynamicLight(const SLight& light)
        {
            const int lidx = SL_LIGHT0 + GetDynamicLightCount();
            GLfloat data[4];

            switch (light.type_)
            {
            case video::ELT_SPOT:
                data[0] = light.direction_.x_;
                data[1] = light.direction_.y_;
                data[2] = light.direction_.z_;
                data[3] = 0.0f;
                SetLightUniform(lidx, SL_LIGHT_DIRECTION, data);

                // set position
                data[0] = light.position_.x_;
                data[1] = light.position_.y_;
                data[2] = light.position_.z_;
                data[3] = 1.0f; // 1.0f for positional light
                SetLightUniform(lidx, SL_LIGHT_POSITION, data);

                SetLightUniform(lidx, SL_LIGHT_EXPONENT, light.falloff_);
                SetLightUniform(lidx, SL_LIGHT_CUTOFF, light.outer_cone_);
                break;
            case video::ELT_POINT:
                // set position
                data[0] = light.position_.x_;
                data[1] = light.position_.y_;
                data[2] = light.position_.z_;
                data[3] = 1.0f; // 1.0f for positional light
                SetLightUniform(lidx, SL_LIGHT_POSITION, data);

                SetLightUniform(lidx, SL_LIGHT_EXPONENT, 0.0f);
                SetLightUniform(lidx, SL_LIGHT_CUTOFF, 180.0f);
                break;
            case video::ELT_DIRECTIONAL:
                // set direction
                data[0] = -light.direction_.x_;
                data[1] = -light.direction_.y_;
                data[2] = -light.direction_.z_;
                data[3] = 0.0f; // 0.0f for directional light
                SetLightUniform(lidx, SL_LIGHT_POSITION, data);

                SetLightUniform(lidx, SL_LIGHT_EXPONENT, 0.0f);
                SetLightUniform(lidx, SL_LIGHT_CUTOFF, 180.0f);
                break;
            default:
                break;
            }

            // set diffuse color
            data[0] = light.diffuse_color_.r;
            data[1] = light.diffuse_color_.g;
            data[2] = light.diffuse_color_.b;
            data[3] = light.diffuse_color_.a;
            SetLightUniform(lidx, SL_LIGHT_DIFFUSE, data);

            // set specular color
            data[0] = light.specular_color_.r;
            data[1] = light.specular_color_.g;
            data[2] = light.specular_color_.b;
            data[3] = light.specular_color_.a;
            SetLightUniform(lidx, SL_LIGHT_SPECULAR, data);

            // set ambient color
            data[0] = light.ambient_color_.r;
            data[1] = light.ambient_color_.g;
            data[2] = light.ambient_color_.b;
            data[3] = light.ambient_color_.a;
            SetLightUniform(lidx, SL_LIGHT_AMBIENT, data);

            // 1.0f / (constant + linear * d + quadratic*(d*d);

            // set attenuation
            data[0] = light.attenuation_.x_;
            data[1] = light.attenuation_.y_;
            data[2] = light.attenuation_.z_;
            SetLightUniform(lidx, SL_LIGHT_ATTENUATION, data);

            Enable(lidx);

            shader_helper_->SetBool("light_on", true);
            return COpenGLDriver::AddDynamicLight(light);
        }

        void COpenGLShaderDriver::SetActiveCameraPosition(core::Vector3Df position) const
        {
            void *pos_cam_world = &position;
            shader_helper_->SetVec4("cam_position", static_cast<f32 *>(pos_cam_world));
        }

        void COpenGLShaderDriver::SetRenderingMode(E_RENDERING_MODE mode)
        {
            rendering_mode_ = mode;
            shader_helper_->SetInt("wireframe_on", mode);
        }

        void COpenGLShaderDriver::BeginShadowRender()
        {
            glViewport(0, 0, shadow_texture_size_.width_, shadow_texture_size_.height_);

            shader_helper_ = shadow_shader_helper_;
            shader_helper_->Use();
            if (shadow_color_texture_ != nullptr && shadow_color_texture_->isFrameBufferObject())
            {
                shadow_color_texture_->bindRTT();
            }

            ClearBuffers(color_buffer_clear_, z_buffer_clear_, false, color_clear_);
            //glEnable(GL_CULL_FACE); // enables face culling    
            //glCullFace(GL_FRONT); // tells OpenGL to cull back faces (the sane default setting)
        }

        void COpenGLShaderDriver::EndShadowRender()
        {
            //glCullFace(GL_BACK); // tells OpenGL to cull back faces (the sane default setting)
            //glDisable(GL_CULL_FACE); // enables face culling    
            shader_helper_ = base_shader_helper_;
            shader_helper_->Use();
            shadow_color_texture_->unbindRTT();

            //glCullFace(GL_BACK); // tells OpenGL to cull back faces (the sane default setting)
            //glDisable(GL_CULL_FACE); // enables face culling    

            DeleteAllDynamicLights();

            glViewport(0, 0, params_.window_size_.width_, params_.window_size_.height_);

            shader_helper_->SetInt(GetUniformName(SL_TEXTURE0 + 4), 4);
            glActiveTexture(GL_TEXTURE0 + 4);
            glBindTexture(GL_TEXTURE_2D, dynamic_cast<const COpenGLTexture*>(shadow_depth_texture_)->GetOpenGLTextureName());
            //shader_helper_->SetBool("shadow_on", true);
        }

        void COpenGLShaderDriver::EnableShadow(bool flag)
        {
#ifdef _DEBUG
            CheckError();
#endif
            COpenGLDriver::EnableShadow(flag);
            base_shader_helper_->SetBool("shadow_on", flag);
#ifdef _DEBUG
            CheckError();
#endif
        }

        void COpenGLShaderDriver::DrawNormalMeshBuffer(const scene::IMeshBuffer* mesh_buffer)
        {
            const S3DVertex *vertices = static_cast<const S3DVertex*>(mesh_buffer->GetVertices());
            u32 vertice_count = mesh_buffer->GetVertexCount();
            const u16 *indices = mesh_buffer->GetIndices();
            const GLsizei indices_count = mesh_buffer->GetIndexCount();

            glBindVertexArray(vao_);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(S3DVertex)* vertice_count, vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16)* indices_count, indices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(S3DVertex), reinterpret_cast<void *>(offsetof(S3DVertex, pos_)));
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(S3DVertex), reinterpret_cast<void *>(offsetof(S3DVertex, normal_)));
            glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(S3DVertex), reinterpret_cast<void *>(offsetof(S3DVertex, color_)));
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(S3DVertex), reinterpret_cast<void *>(offsetof(S3DVertex, texcoord_)));

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glEnableVertexAttribArray(3);

            if (rendering_mode_ == ERM_WIREFRAME)
            {
                glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(S3DVertex), reinterpret_cast<void *>(offsetof(S3DVertex, barycentric_)));
                glEnableVertexAttribArray(4);
            }

            shader_helper_->Use();
            glBindVertexArray(vao_);
            glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_SHORT, nullptr);
            glBindVertexArray(0);
        }

        void COpenGLShaderDriver::DrawTangentMeshBuffer(const scene::IMeshBuffer* mesh_buffer)
        {
            const S3DVertexTangents *vertices = static_cast<const S3DVertexTangents*>(mesh_buffer->GetVertices());
            u32 vertice_count = mesh_buffer->GetVertexCount();
            const u16 *indices = mesh_buffer->GetIndices();
            const GLsizei indices_count = mesh_buffer->GetIndexCount();

            glBindVertexArray(vao_);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(S3DVertexTangents)* vertice_count, vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16)* indices_count, indices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(S3DVertexTangents), reinterpret_cast<void *>(offsetof(S3DVertexTangents, pos_)));
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(S3DVertexTangents), reinterpret_cast<void *>(offsetof(S3DVertexTangents, normal_)));
            glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(S3DVertexTangents), reinterpret_cast<void *>(offsetof(S3DVertexTangents, color_)));
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(S3DVertexTangents), reinterpret_cast<void *>(offsetof(S3DVertexTangents, texcoord_)));
            glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(S3DVertexTangents), reinterpret_cast<void *>(offsetof(S3DVertexTangents, tangent_)));
            glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(S3DVertexTangents), reinterpret_cast<void *>(offsetof(S3DVertexTangents, binormal_)));

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glEnableVertexAttribArray(3);
            glEnableVertexAttribArray(5);
            glEnableVertexAttribArray(6);

            if (rendering_mode_ == ERM_WIREFRAME)
            {
                glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(S3DVertexTangents), reinterpret_cast<void *>(offsetof(S3DVertexTangents, barycentric_)));
                glEnableVertexAttribArray(4);
            }

            shader_helper_->Use();
            glBindVertexArray(vao_);
            glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_SHORT, nullptr);
            glBindVertexArray(0);
        }

        void COpenGLShaderDriver::UpdateMaxSupportLights()
        {
            max_support_lights_ = 4;
        }

        void COpenGLShaderDriver::SetMaterialUniform(s32 material_val_type, const void *val) const
        {
            if (material_val_type >= SL_MATERIAL_COUNT || material_val_type < 0)
            {
                return;
            }
            switch (material_val_type)
            {
            case SL_MAT_AMBIENT:
            case SL_MAT_DIFFUSE:
            case SL_MAT_SPECULAR:
            case SL_MAT_EMISSIVE:
                shader_helper_->SetVec4(material_uniform_name[material_val_type], static_cast<const f32 *>(val));
                break;
            case SL_MAT_SHININESS:
                shader_helper_->SetFloat(material_uniform_name[material_val_type], *static_cast<const f32 *>(val));
            default: break;
            }
            
        }

        void COpenGLShaderDriver::SetMaterialUniform(s32 material_val_type, f32 val) const
        {
            if (material_val_type >= SL_MATERIAL_COUNT || material_val_type < 0)
            {
                return;
            }

            shader_helper_->SetFloat(material_uniform_name[material_val_type], val);
        }

        void COpenGLShaderDriver::SetLightUniform(s32 light_idx, s32 light_val_type, const void* val) const
        {
            if (light_idx < SL_LIGHT0 || light_idx >= SL_LIGHT0 + max_support_lights_ || 
                light_val_type < 0 || light_val_type >= SL_LIGHT_COUNT)
            {
                return;
            }

            core::stringc str = core::stringc(shader_uniform_name[light_idx]) + "." + light_uniform_name[light_val_type];
            //os::Printer::print(str.c_str());

            switch (light_val_type)
            {
            case SL_LIGHT_POSITION:
            case SL_LIGHT_DIRECTION:
            case SL_LIGHT_AMBIENT:
            case SL_LIGHT_DIFFUSE:
            case SL_LIGHT_SPECULAR:
            case SL_LIGHT_ATTENUATION:
                shader_helper_->SetVec4(str.c_str(), static_cast<const f32 *>(val));
                break;
            case SL_LIGHT_EXPONENT:
            case SL_LIGHT_CUTOFF:
                shader_helper_->SetFloat(str.c_str(), *static_cast<const f32 *>(val));
            default: break;
            }
        }

        void COpenGLShaderDriver::SetLightUniform(s32 light_idx, s32 light_val_type, f32 val) const
        {
            if (light_idx < SL_LIGHT0 || light_idx >= SL_LIGHT0 + max_support_lights_ ||
                light_val_type < 0 || light_val_type >= SL_LIGHT_COUNT)
            {
                return;
            }

            core::stringc str = core::stringc(shader_uniform_name[light_idx]) + "." + light_uniform_name[light_val_type];
            shader_helper_->SetFloat(str.c_str(), val);
        }

        void COpenGLShaderDriver::Enable(s32 idx) const
        {
            if (idx < 0 || idx >= SL_COUNT)
            {
                return;
            }

            core::stringc str = core::stringc(shader_uniform_name[idx]) + "_on";
            shader_helper_->SetBool(str.c_str(), true);
        }

        void COpenGLShaderDriver::Disable(s32 idx) const
        {
            if (idx < 0 || idx >= SL_COUNT)
            {
                return;
            }

            core::stringc str = core::stringc(shader_uniform_name[idx]) + "_on";
            shader_helper_->SetBool(str.c_str(), false);
        }

        const c8* COpenGLShaderDriver::GetUniformName(s32 idx) const
        {
            if (idx < 0 || idx >= SL_COUNT)
            {
                return nullptr;
            }

            return shader_uniform_name[idx];
        }
    } // end namespace video
} // end namespace kong

namespace kong
{
    namespace video
    {
#ifdef _KONG_COMPILE_WITH_OPENGL_
        IVideoDriver* CreateOpenGLShaderDriver(const kong::SKongCreationParameters& params,
            io::IFileSystem* io, CKongDeviceWin32* device)
        {
            COpenGLShaderDriver *driver = new COpenGLShaderDriver(params, io, device);
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