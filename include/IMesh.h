// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _IMESH_H_
#define _IMESH_H_

#include "SMaterial.h"

namespace kong
{
    namespace scene
    {
        class IMeshBuffer;

        class IMesh
        {
        public:
            //! virtual deconstructor
            virtual ~IMesh() = default;

            //! Get the amount of mesh buffers.
            /** \return Amount of mesh buffers (IMeshBuffer) in this mesh. */
            virtual u32 GetMeshBufferCount() const = 0;

            //! Get pointer to a mesh buffer.
            /** \param nr: Zero based index of the mesh buffer. The maximum value is
            getMeshBufferCount() - 1;
            \return Pointer to the mesh buffer or 0 if there is no such
            mesh buffer. */
            virtual IMeshBuffer* GetMeshBuffer(u32 nr) const = 0;

            //! Get pointer to a mesh buffer which fits a material
            /** \param material: material to search for
            \return Pointer to the mesh buffer or 0 if there is no such
            mesh buffer. */
            virtual IMeshBuffer* GetMeshBuffer(const video::SMaterial &material) const = 0;
        };
    } // end namespace scene
} // end namespace kong

#endif