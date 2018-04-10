// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _CREADFILE_H_
#define _CREADFILE_H_

#include "IReadFile.h"
#include "SPath.h"

namespace kong
{
    namespace io
    {

        /*!
        Class for reading a real file from disk.
        */
        class CReadFile : public IReadFile
        {
        public:

            CReadFile(const io::SPath &fileName);

            virtual ~CReadFile();

            //! returns how much was read
            s32 Read(void* buffer, u32 size_to_read) override;

            //! changes position in file, returns true if successful
            bool Seek(long finalPos, bool relative_movement = false) override;

            //! returns size of file
            long GetSize() const override;

            //! returns if file is open
            virtual bool IsOpen() const;

            //! returns where in the file we are.
            long GetPos() const override;

            //! returns name of file
            const io::SPath& GetFileName() const override;

        private:

            //! opens the file
            void OpenFile();

            FILE* file_;
            long file_size_;
            io::SPath filename_;
        };

    } // end namespace io
} // end namespace irr

#endif

