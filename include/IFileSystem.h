// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _IFILESYSTEM_H_
#define _IFILESYSTEM_H_
#include <string>

#include "KongTypes.h"

namespace kong
{
    namespace io
    {
        typedef std::string path;

        class IReadFile;
        class IWriteFile;

        class IFileSystem
        {
        public:
            virtual ~IFileSystem() = default;

        private:
            //! Opens a file for read access.
            /** \param filename: Name of file to open.
            \return Pointer to the created file interface.
            The returned pointer should be dropped when no longer needed.
            See IReferenceCounted::drop() for more information. */
            virtual IReadFile* CreateAndOpenFile(const path filename) = 0;

            //! Creates an IReadFile interface for accessing memory like a file.
            /** This allows you to use a pointer to memory where an IReadFile is requested.
            \param memory: A pointer to the start of the file in memory
            \param len: The length of the memory in bytes
            \param fileName: The name given to this file
            \param deleteMemoryWhenDropped: True if the memory should be deleted
            along with the IReadFile when it is dropped.
            \return Pointer to the created file interface.
            The returned pointer should be dropped when no longer needed.
            See IReferenceCounted::drop() for more information.
            */
            virtual IReadFile* CreateMemoryReadFile(void* memory, s32 len, const path fileName, bool deleteMemoryWhenDropped = false) = 0;

            //! Opens a file for write access.
            /** \param filename: Name of file to open.
            \param append: If the file already exist, all write operations are
            appended to the file.
            \return Pointer to the created file interface. 0 is returned, if the
            file could not created or opened for writing.
            The returned pointer should be dropped when no longer needed.
            See IReferenceCounted::drop() for more information. */
            virtual IWriteFile* CreateAndWriteFile(const path filename, bool append = false) = 0;

            //! Converts a relative path to an absolute (unique) path, resolving symbolic links if required
            /** \param filename Possibly relative file or directory name to query.
            \result Absolute filename which points to the same file. */
            virtual path getAbsolutePath(const path& filename) const = 0;

            //! Get the directory a file is located in.
            /** \param filename: The file to get the directory from.
            \return String containing the directory of the file. */
            virtual path getFileDir(const path& filename) const = 0;
        };
    }
}

#endif