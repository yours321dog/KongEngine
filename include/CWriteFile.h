// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".
#ifndef _CWRITEFILE_H_
#define _CWRITEFILE_H_

#include "IWriteFile.h"
#include "SPath.h"

namespace kong
{

    namespace io
    {

        /*!
        Class for writing a real file to disk.
        */
        class CWriteFile : public IWriteFile
        {
        public:

            CWriteFile(const io::SPath& filename, bool append);

            virtual ~CWriteFile();

            //! Reads an amount of bytes from the file.
            s32 Write(const void* buffer, u32 size_to_write) override;

            //! Changes position in file, returns true if successful.
            bool Seek(long finalPos, bool relative_movement = false) override;

            //! Returns the current position in the file.
            long GetPos() const override;

            //! Returns name of file.
            const io::SPath& GetFileName() const override;

            //! returns if file is open
            bool IsOpen() const;

        private:

            //! opens the file
            void OpenFile(bool append);

            io::SPath filename_;
            FILE* file_;
            long file_size_;
        };

    } // end namespace io
} // end namespace kong

#endif

