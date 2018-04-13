// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _CFILESYSTEM_H_
#define _CFILESYSTEM_H_

#include "IFileSystem.h"

namespace kong
{
    namespace io
    {
        class CFileSystem : public IFileSystem
        {
        public :
            virtual ~CFileSystem() = default;

            //! opens a file for read access
            IReadFile* CreateAndOpenFile(const SPath &filename) override;
            
            //! Creates an IReadFile interface for accessing memory like a file.
            IReadFile* CreateMemoryReadFile(void* memory, s32 len, const SPath &fileName, bool deleteMemoryWhenDropped) override;
            
            //! Opens a file for write access.
            IWriteFile* CreateAndWriteFile(const SPath &filename, bool append) override;
            
            //! Converts a relative path to an absolute (unique) path, resolving symbolic links if required
            SPath GetAbsolutePath(const SPath& filename) const override;
            
            //! Get the directory a file is located in.
            SPath GetFileDir(const SPath& filename) const override;

            //! Get the base name of a file
            SPath GetFileBasename(const SPath& filename, bool keepExtension) const override;

            //! Determines if a file exists and could be opened.
            bool ExistFile(const path& filename) const override;
        };
    } // end namepsace io
} // end namespace kong

#endif