// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#include "CReadFile.h"


namespace kong
{
    namespace io
    {
        CReadFile::CReadFile(const io::SPath &filename)
            : file_(nullptr), file_size_(-1), filename_(filename)
        {
            OpenFile();
        }

        CReadFile::~CReadFile()
        {
            if (file_ != nullptr)
            {
                fclose(file_);
            }
        }

        s32 CReadFile::Read(void* buffer, u32 size_to_read)
        {
            if (!IsOpen())
            {
                return 0;
            }

            return static_cast<s32>(fread(buffer, 1, size_to_read, file_));
        }

        bool CReadFile::Seek(long finalPos, bool relative_movement)
        {
            if (!IsOpen())
            {
                return false;
            }

            return fseek(file_, finalPos, relative_movement ? SEEK_CUR : SEEK_SET) == 0;
        }

        long CReadFile::GetSize() const
        {
            return file_size_;
        }

        bool CReadFile::IsOpen() const
        {
            return file_ != 0;
        }

        long CReadFile::GetPos() const
        {
            return ftell(file_);
        }

        const SPath& CReadFile::GetFileName() const
        {
            return filename_;
        }

        void CReadFile::OpenFile()
        {
            if (filename_.empty()) // bugfix posted by rt
            {
                file_ = nullptr;
                return;
            }

#if defined ( _KONG_WCHAR_FILESYSTEM )
            file_ = _wfopen(filename_.c_str(), L"rb");
#else
            fopen_s(&file_, filename_.c_str(), "rb");
#endif

            if (file_)
            {
                // get FileSize
                fseek(file_, 0, SEEK_END);
                file_size_ = GetPos();
                fseek(file_, 0, SEEK_SET);
            }
        }

        IReadFile* CreateReadFile(const io::SPath& filename)
        {
            CReadFile* file = new CReadFile(filename);
            if (file->IsOpen())
            {
                return file;
            }

            return nullptr;
        }

    } // end namespace io
} // end namespace kong