// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _SVIEWFRUSTUM_H_
#define _SVIEWFRUSTUM_H_
#include "Vector.h"
#include "Matrix.h"

namespace kong
{
    namespace scene
    {
        class SViewFrustum
        {
            enum VFPLANES
            {
                //! Far plane of the frustum. That is the plane farest away from the eye.
                VF_FAR_PLANE = 0,
                //! Near plane of the frustum. That is the plane nearest to the eye.
                VF_NEAR_PLANE,
                //! Left plane of the frustum.
                VF_LEFT_PLANE,
                //! Right plane of the frustum.
                VF_RIGHT_PLANE,
                //! Bottom plane of the frustum.
                VF_BOTTOM_PLANE,
                //! Top plane of the frustum.
                VF_TOP_PLANE,

                //! Amount of planes enclosing the view frustum. Should be 6.
                VF_PLANE_COUNT
            };

            SViewFrustum();

            //! Copy Constructor
            SViewFrustum(const SViewFrustum& other);

            //! This constructor creates a view frustum based on a projection and/or view matrix.
            SViewFrustum(const core::Matrixf& mat);

            core::Vector3Df camera_position_;

        private:
            //! Hold a copy of important transform matrices
            enum E_TRANSFORMATION_STATE_FRUSTUM
            {
                ETS_VIEW = 0,
                ETS_PROJECTION = 1,
                ETS_COUNT_FRUSTUM
            };

            core::Matrixf matrices[ETS_COUNT_FRUSTUM];
        };

        inline SViewFrustum::SViewFrustum()
        = default;

        inline SViewFrustum::SViewFrustum(const SViewFrustum& other)
        {
            camera_position_ = other.camera_position_;

            for (u32 i = 0; i < ETS_COUNT_FRUSTUM; i++)
            {
                matrices[i] = other.matrices[i];
            }
        }

        inline SViewFrustum::SViewFrustum(const core::Matrixf& mat)
        {
        }
    }
}

#endif