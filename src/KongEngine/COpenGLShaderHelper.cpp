// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "COpenGLShaderHelper.h"
#include "IReadFile.h"
#include "KongMath.h"
#include "GL/glew.h"
#include <iostream>

namespace kong
{
    namespace video
    {
        COpenGLShaderHelper::COpenGLShaderHelper(io::IFileSystem* file_system, const io::SPath& vertex_path,
            const io::SPath& fragment_path)
            : id_(0), file_system_(file_system)
        {
            InitShader(vertex_path, fragment_path);
        }

        void COpenGLShaderHelper::Use()
        {
            glUseProgram(id_);
        }

        void COpenGLShaderHelper::SetBool(const std::string& name, bool value) const
        {
            const GLint location = glGetUniformLocation(id_, name.c_str());
            if (location >= 0)
                glUniform1i(location, (value));
        }

        void COpenGLShaderHelper::SetInt(const std::string& name, s32 value) const
        {
            const GLint location = glGetUniformLocation(id_, name.c_str());
            if (location >= 0)
                glUniform1i(location, value);
        }

        void COpenGLShaderHelper::SetFloat(const std::string& name, f32 value) const
        {
            const GLint location = glGetUniformLocation(id_, name.c_str());
            if (location >= 0)
                glUniform1f(location, value);
        }

        void COpenGLShaderHelper::SetMatrix4(const std::string& name, const core::Matrixf& mat) const
        {
            const GLint location = glGetUniformLocation(id_, name.c_str());
            if (location >= 0)
                glUniformMatrix4fv(location, 1, GL_FALSE, mat.Pointer());
        }

        void COpenGLShaderHelper::SetVec4(const std::string& name, const f32* vec4) const
        {
            const GLint location = glGetUniformLocation(id_, name.c_str());
            if (location >= 0)
                glUniform4fv(location, 1, vec4);
        }

        void COpenGLShaderHelper::InitShader(const io::SPath& vertex_path, const io::SPath& fragment_path)
        {
            // 1. read shader files
            std::string vertex_code;
            std::string fragment_code;

            io::IReadFile *vs_file = file_system_->CreateAndOpenFile(vertex_path);
            io::IReadFile *fs_file = file_system_->CreateAndOpenFile(fragment_path);

            c8 *vs_code = new c8[vs_file->GetSize() + 1];
            c8 *fs_code = new c8[fs_file->GetSize() + 1];

            vs_file->Read(vs_code, vs_file->GetSize());
            fs_file->Read(fs_code, fs_file->GetSize());
            vs_code[vs_file->GetSize()] = '\0';
            fs_code[fs_file->GetSize()] = '\0';
 
            // 2. compile shaders
            s32 success;
            c8 infoLog[512];

            // vertex shader
            const u32 vertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex, 1, &vs_code, nullptr);
            glCompileShader(vertex);
            // check shader result
            glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
                std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            };

            // fragment shader
            const u32 fragment = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment, 1, &fs_code, nullptr);
            glCompileShader(fragment);
            // check shader result
            glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
                std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            };

            // link shaders
            id_ = glCreateProgram();
            glAttachShader(id_, vertex);
            glAttachShader(id_, fragment);
            glLinkProgram(id_);
            // check link result
            glGetProgramiv(id_, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(id_, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            }

            // delete shaders
            glDeleteShader(vertex);
            glDeleteShader(fragment);

            delete[] vs_code;
            delete[] fs_code;
            delete vs_file;
            delete fs_file;
        }
    } // end namespace video
} // end namespace kong