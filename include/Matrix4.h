// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _MATRIX4_H_
#define _MATRIX4_H_

#include "KongTypes.h"

namespace kong
{
    namespace core
    {
        template <class T>
        class CMatrix4
        {

        };

        //! Typedef for f32 matrix
        typedef CMatrix4<f32> matrix4;

        //! global const identity matrix
        KONG_API extern const matrix4 IdentityMatrix;
    } // end namespace core
} // end namespace kong

#endif