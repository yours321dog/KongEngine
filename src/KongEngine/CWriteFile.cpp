// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "CWriteFile.h"

namespace kong
{
    namespace io
    {
        CWriteFile::CWriteFile(const io::SPath& filename, bool append)
            : filename_(filename), file_(nullptr), file_size_(0)
        {
            OpenFile(append);
        }

        CWriteFile::~CWriteFile()
        {
            if (file_)
            {
                fclose(file_);
            }
        }

        s32 CWriteFile::Write(const void* buffer, u32 size_to_write)
        {
            if (!IsOpen())
            {
                return 0;
            }

            return static_cast<s32>(fwrite(buffer, 1, size_to_write, file_));
        }

        bool CWriteFile::Seek(long final_pos, bool relative_movement)
        {
            if (!IsOpen())
            {
                return false;
            }

            return fseek(file_, final_pos, relative_movement ? SEEK_CUR : SEEK_SET) == 0;
        }

        long CWriteFile::GetPos() const
        {
            return ftell(file_);
        }

        const io::SPath& CWriteFile::GetFileName() const
        {
            return filename_;
        }

        bool CWriteFile::IsOpen() const
        {
            return file_ != nullptr;
        }

        void CWriteFile::OpenFile(bool append)
        {
            if (filename_.empty())
            {
                file_ = nullptr;
                return;
            }

#if defined(_KONG_WCHAR_FILESYSTEM)
            file_ = _wfopen(filename_.c_str(), append ? L"ab" : L"wb");
#else
            fopen_s(&file_, filename_.c_str(), append ? "ab" : "wb");
#endif
            if (file_ != nullptr)
            {
                // get FileSize
                fseek(file_, 0, SEEK_END);
                file_size_ = ftell(file_);
                fseek(file_, 0, SEEK_SET);
            }
        }

        IWriteFile* CreateWriteFile(const io::SPath& filename, bool append)
        {
            CWriteFile* file = new CWriteFile(filename, append);
            if (file->IsOpen())
            {
                return file;
            }

            return nullptr;
        }
    } // end namespace io
} // end namespace kong