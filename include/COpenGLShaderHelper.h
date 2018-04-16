// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _OPENGLSHADERHELPTER_H_
#define _OPENGLSHADERHELPTER_H_

#include "SPath.h"
#include "IShaderHelper.h"
#include "IFileSystem.h"

namespace kong
{
    namespace video
    {
        class COpenGLShaderHelper : public IShaderHelper
        {
        public:
            // constructor
            COpenGLShaderHelper(io::IFileSystem *file_system, const io::SPath &vertex_path, const io::SPath &fragment_path);

            // activate the shader
            void Use() override;
            
            // uniform set functions
            void SetBool(const std::string &name, bool value) const override;
            void SetInt(const std::string &name, s32 value) const override;
            void SetFloat(const std::string &name, f32 value) const override;
            void SetMatrix4(const std::string &name, const core::Matrixf &mat) const override;

        private:
            void InitShader(const io::SPath &vertex_path, const io::SPath &fragment_path);

            // program id
            unsigned int id_;

            // file system
            io::IFileSystem *file_system_;
        };
    } // end namespace video
} // end namespace kong

#endif