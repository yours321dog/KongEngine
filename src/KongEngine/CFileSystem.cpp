// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "CFileSystem.h"

namespace kong
{
    namespace io
    {
        IReadFile* CFileSystem::CreateAndOpenFile(const path filename)
        {
            return nullptr;
        }

        path CFileSystem::getAbsolutePath(const path& filename) const
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
            std::string tmp(p);
            int idx = -1;
            while ((idx = tmp.find('\\')) >= 0)
            {
                tmp.replace(idx, 1, "/");
            }
#endif
            return tmp;
#elif (defined(_IRR_POSIX_API_) || defined(_IRR_OSX_PLATFORM_))
            c8* p = 0;
            c8 fpath[4096];
            fpath[0] = 0;
            p = realpath(filename.c_str(), fpath);
            if (!p)
            {
                // content in fpath is unclear at this point
                if (!fpath[0]) // seems like fpath wasn't altered, use our best guess
                {
                    io::path tmp(filename);
                    return flattenFilename(tmp);
                }
                else
                    return io::path(fpath);
            }
            if (filename[filename.size() - 1] == '/')
                return io::path(p) + _IRR_TEXT("/");
            else
                return io::path(p);
#else
            return io::path(filename);
#endif
        }
    } // end namespace io
} // end namespace kong