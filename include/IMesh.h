// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _IMESH_H_
#define _IMESH_H_

#include "SMaterial.h"
#include "aabbox3d.h"
#include "EHardwareBufferFlags.h"

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

            //! Get an axis aligned bounding box of the mesh.
            /** \return Bounding box of this mesh. */
            virtual const core::aabbox3d<f32>& GetBoundingBox() const = 0;

            //! Set user-defined axis aligned bounding box
            /** \param box New bounding box to use for the mesh. */
            virtual void SetBoundingBox(const core::aabbox3df& box) = 0;

            //! Sets a flag of all contained materials to a new value.
            /** \param flag: Flag to set in all materials.
            \param newvalue: New value to set in all materials. */
            virtual void SetMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue) = 0;

            //! Set the hardware mapping hint
            /** This methods allows to define optimization hints for the
            hardware. This enables, e.g., the use of hardware buffers on
            pltforms that support this feature. This can lead to noticeable
            performance gains. */
            virtual void SetHardwareMappingHint(E_HARDWARE_MAPPING newMappingHint, E_BUFFER_TYPE buffer = EBT_VERTEX_AND_INDEX) = 0;

            //! Flag the meshbuffer as changed, reloads hardware buffers
            /** This method has to be called every time the vertices or
            indices have changed. Otherwise, changes won't be updated
            on the GPU in the next render cycle. */
            virtual void SetDirty(E_BUFFER_TYPE buffer = EBT_VERTEX_AND_INDEX) = 0;
        };
    } // end namespace scene
} // end namespace kong

#endif