// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "CFileSystem.h"
#include "IReadFile.h"
#include "IWriteFile.h"

namespace kong
{
    namespace io
    {
        IReadFile* CFileSystem::CreateAndOpenFile(const SPath &filename)
        {
            return CreateReadFile(filename);
        }

        IReadFile* CFileSystem::CreateMemoryReadFile(void* memory, s32 len, const SPath& fileName,
            bool deleteMemoryWhenDropped)
        {
            return nullptr;
        }

        IWriteFile* CFileSystem::CreateAndWriteFile(const SPath& filename, bool append)
        {
            return CreateWriteFile(filename, append);
        }

        SPath CFileSystem::GetAbsolutePath(const SPath& filename) const
        {
#if defined(_KONG_WINDOWS_CE_PLATFORM_)
            return filename;
#elif defined(_KONG_WINDOWS_API_)
            fschar_t *p = 0;
            fschar_t fpath[_MAX_PATH];
#if defined(_KONG_WCHAR_FILESYSTEM )
            p = _wfullpath(fpath, filename.c_str(), _MAX_PATH);
            core::stringw tmp(p);
            tmp.replace(L'\\', L'/');
#else
            p = _fullpath(fpath, filename.c_str(), _MAX_PATH);
            core::stringc tmp(p);
            tmp.replace('\\', '/');
#endif
            return tmp;
#elif (defined(_KONG_POSIX_API_) || defined(_KONG_OSX_PLATFORM_))
            c8* p = 0;
            c8 fpath[4096];
            fpath[0] = 0;
            p = realpath(filename.c_str(), fpath);
            if (!p)
            {
                // content in fpath is unclear at this point
                if (!fpath[0]) // seems like fpath wasn't altered, use our best guess
                {
                    io::SPath tmp(filename);
                    return flattenFilename(tmp);
                }
                else
                    return io::SPath(fpath);
            }
            if (filename[filename.size() - 1] == '/')
                return io::SPath(p) + _KONG_TEXT("/");
            else
                return io::SPath(p);
#else
            return io::SPath(filename);
#endif
        }

        SPath CFileSystem::GetFileDir(const SPath& filename) const
        {
            // find last forward or backslash
            s32 lastSlash = filename.findLast('/');
            const s32 lastBackSlash = filename.findLast('\\');
            lastSlash = lastSlash > lastBackSlash ? lastSlash : lastBackSlash;

            if (static_cast<u32>(lastSlash) < filename.size())
                return filename.subString(0, lastSlash);
            else
                return _KONG_TEXT(".");
        }

        SPath CFileSystem::GetFileBasename(const SPath& filename, bool keepExtension) const
        {
            // find last forward or backslash
            s32 last_slash = filename.findLast('/');
            const s32 last_back_slash = filename.findLast('\\');
            last_slash = core::max_(last_slash, last_back_slash);

            // get number of chars after last dot
            s32 end = 0;
            if (!keepExtension)
            {
                // take care to search only after last slash to check only for
                // dots in the filename
                end = filename.findLast('.');
                if (end == -1 || end < last_slash)
                    end = 0;
                else
                    end = filename.size() - end;
            }

            if (static_cast<u32>(last_slash) < filename.size())
                return filename.subString(last_slash + 1, filename.size() - last_slash - 1 - end);
            else if (end != 0)
                return filename.subString(0, filename.size() - end);
            else
                return filename;
        }

        //! creates a filesystem which is able to open files from the ordinary file system,
        //! and out of zipfiles, which are able to be added to the filesystem.
        IFileSystem* CreateFileSystem()
        {
            return new CFileSystem();
        }
    } // end namespace io
} // end namespace kong