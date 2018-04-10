// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _IWRITEFILE_H_
#define _IWRITEFILE_H_

#include "SPath.h"
#include "KongTypes.h"

namespace kong
{
    namespace io
    {

        //! Interface providing write access to a file.
        class IWriteFile
        {
        public:
            virtual ~IWriteFile() = default;
            //! Writes an amount of bytes to the file.
            /** \param buffer Pointer to buffer of bytes to write.
            \param size_to_write Amount of bytes to write to the file.
            \return How much bytes were written. */
            virtual s32 Write(const void* buffer, u32 size_to_write) = 0;

            //! Changes position in file
            /** \param final_pos Destination position in the file.
            \param relative_movement If set to true, the position in the file is
            changed relative to current position. Otherwise the position is changed
            from begin of file.
            \return True if successful, otherwise false. */
            virtual bool Seek(long final_pos, bool relative_movement = false) = 0;

            //! Get the current position in the file.
            /** \return Current position in the file in bytes. */
            virtual long GetPos() const = 0;

            //! Get name of file.
            /** \return File name as zero terminated character string. */
            virtual const SPath& GetFileName() const = 0;
        };

        //! Internal function, please do not use.
        IWriteFile* CreateWriteFile(const io::SPath& filename, bool append);

    } // end namespace io
} // end namespace irr

#endif

