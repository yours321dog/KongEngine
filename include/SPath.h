// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _SPATH_H_
#define _SPATH_H_

//#include <string>
#include "KongString.h"

namespace kong
{
    namespace io
    {
        typedef core::stringc SPath;

        //! Type used for all file system related strings.
        /** This type will transparently handle different file system encodings. */
        typedef core::string<fschar_t> path;

        //! Used in places where we identify objects by a filename, but don't actually work with the real filename
        /** Irrlicht is internally not case-sensitive when it comes to names.
        Also this class is a first step towards support for correctly serializing renamed objects.
        */
        struct SNamedPath
        {
            //! Constructor
            SNamedPath() {}

            //! Constructor
            SNamedPath(const path& p) : path_(p), internal_name_(PathToName(p))
            {
            }

            //! Is smaller comparator
            bool operator <(const SNamedPath& other) const
            {
                return internal_name_ < other.internal_name_;
            }

            //! Set the path.
            void SetPath(const path& p)
            {
                path_ = p;
                internal_name_ = PathToName(p);
            }

            //! Get the path.
            const path& GetPath() const
            {
                return path_;
            };

            //! Get the name which is used to identify the file.
            //! This string is similar to the names and filenames used before Irrlicht 1.7
            const path& GetInternalName() const
            {
                return internal_name_;
            }

            //! Implicit cast to io::path
            operator core::stringc() const
            {
                return core::stringc(GetPath());
            }
            //! Implicit cast to io::path
            operator core::stringw() const
            {
                return core::stringw(GetPath());
            }

        protected:
            // convert the given path string to a name string.
            path PathToName(const path& p) const
            {
                path name(p);
                name.replace('\\', '/');
                name.make_lower();
                return name;
            }

        private:
            path path_;
            path internal_name_;
        };
    } // end namespace io
} // end namespace kong
#endif