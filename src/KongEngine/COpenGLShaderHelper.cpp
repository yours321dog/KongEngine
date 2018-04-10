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
        COpengGLShaderHelper::COpengGLShaderHelper(io::IFileSystem* file_system, const io::SPath& vertex_path,
            const io::SPath& fragment_path)
            : id_(0), file_system_(file_system)
        {
            InitShader(vertex_path, fragment_path);
        }

        void COpengGLShaderHelper::InitShader(const io::SPath& vertex_path, const io::SPath& fragment_path)
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

            // 2. 编译着色器
            s32 success;
            c8 infoLog[512];

            // 顶点着色器
            const u32 vertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex, 1, &vs_code, nullptr);
            glCompileShader(vertex);
            // 打印编译错误（如果有的话）
            glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
                std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            };

            // 片段着色器也类似
            const u32 fragment = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment, 1, &fs_code, nullptr);
            glCompileShader(fragment);
            // 打印编译错误（如果有的话）
            glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
                std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            };

            // 着色器程序
            id_ = glCreateProgram();
            glAttachShader(id_, vertex);
            glAttachShader(id_, fragment);
            glLinkProgram(id_);
            // 打印连接错误（如果有的话）
            glGetProgramiv(id_, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(id_, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            }

            // 删除着色器，它们已经链接到我们的程序中了，已经不再需要了
            glDeleteShader(vertex);
            glDeleteShader(fragment);

            delete[] vs_code;
            delete[] fs_code;
            delete vs_file;
            delete fs_file;
        }
    } // end namespace video
} // end namespace kong