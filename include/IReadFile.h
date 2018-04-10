// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".


#ifndef _IREADFILE_H_
#define _IREADFILE_H_

#include "KongTypes.h"
#include "SPath.h"

namespace kong
{
    namespace io
    {

        //! Interface providing read acess to a file.
        class IReadFile
        {
        public:
            virtual ~IReadFile() = default;
            //! Reads an amount of bytes from the file.
            /** \param buffer Pointer to buffer where read bytes are written to.
            \param size_to_read Amount of bytes to read from the file.
            \return How many bytes were read. */
            virtual s32 Read(void* buffer, u32 size_to_read) = 0;

            //! Changes position in file
            /** \param finalPos Destination position in the file.
            \param relative_movement If set to true, the position in the file is
            changed relative to current position. Otherwise the position is changed
            from beginning of file.
            \return True if successful, otherwise false. */
            virtual bool Seek(long finalPos, bool relative_movement = false) = 0;

            //! Get size of file.
            /** \return Size of the file in bytes. */
            virtual long GetSize() const = 0;

            //! Get the current position in the file.
            /** \return Current position in the file in bytes. */
            virtual long GetPos() const = 0;

            //! Get name of file.
            /** \return File name as zero terminated character string. */
            virtual const SPath& GetFileName() const = 0;
        };

        //! Internal function, please do not use.
        IReadFile* CreateReadFile(const SPath& filename);
        //! Internal function, please do not use.
        IReadFile* CreateLimitReadFile(const io::SPath& filename, IReadFile* already_opened_file, long pos, long area_size);
        //! Internal function, please do not use.
        IReadFile* CreateMemoryReadFile(void* memory, long size, const io::SPath& filename, bool delete_memory_when_dropped);

    } // end namespace io
} // end namespace irr

#endif

