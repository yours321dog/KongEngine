// Copyright (C) 2002-2012 Nikolaus Gebhardt, 2018 Luan Lyu
// This file is part of the "Kong Engine".

#ifndef _IIMAGELOADER_H_
#define _IIMAGELOADER_H_

#include "IImage.h"
#include "SPath.h"

namespace kong
{
    namespace io
    {
        class IReadFile;
    } // end namespace io
    namespace video
    {

        //! Class which is able to create a image from a file.
        /** If you want the Irrlicht Engine be able to load textures of
        currently unsupported file formats (e.g .gif), then implement
        this and add your new Surface loader with
        IVideoDriver::addExternalImageLoader() to the engine. */
        class IImageLoader
        {
        public:
            virtual ~IImageLoader() = default;

            //! Check if the file might be loaded by this class
            /** Check is based on the file extension (e.g. ".tga")
            \param filename Name of file to check.
            \return True if file seems to be loadable. */
            virtual bool IsALoadableFileExtension(const io::SPath& filename) const = 0;

            //! Check if the file might be loaded by this class
            /** Check might look into the file.
            \param file File handle to check.
            \return True if file seems to be loadable. */
            virtual bool IsALoadableFileFormat(io::IReadFile* file) const = 0;

            //! Creates a surface from the file
            /** \param file File handle to check.
            \return Pointer to newly created image, or 0 upon error. */
            virtual IImage* LoadImage(io::IReadFile* file) const = 0;
        };


    } // end namespace video
} // end namespace irr

#endif

