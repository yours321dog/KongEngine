// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _SMESH_H_
#define _SMESH_H_

#include "IMesh.h"
#include "Array.h"
#include "IMeshBuffer.h"

namespace kong
{
    namespace scene
    {
        class SMesh : public IMesh
        {
        public:
            //! destructor
            virtual ~SMesh();

            virtual void Clear();
                
            //! Get the amount of mesh buffers.
            /** \return Amount of mesh buffers (IMeshBuffer) in this mesh. */
            u32 GetMeshBufferCount() const override;

            //! Get pointer to a mesh buffer.
            /** \param nr: Zero based index of the mesh buffer. The maximum value is
            getMeshBufferCount() - 1;
            \return Pointer to the mesh buffer or 0 if there is no such
            mesh buffer. */
            IMeshBuffer* GetMeshBuffer(u32 nr) const override;

            //! Get pointer to a mesh buffer which fits a material
            /** \param material: material to search for
            \return Pointer to the mesh buffer or 0 if there is no such
            mesh buffer. */
            IMeshBuffer* GetMeshBuffer(const video::SMaterial &material) const override;

            //! recalculates the bounding box
            void RecalculateBoundingBox();

            void AddMeshBuffer(IMeshBuffer *mesh_buffer);

            //! returns an axis aligned bounding box
            const core::aabbox3d<f32>& GetBoundingBox() const override;

            //! set user axis aligned bounding box
            void SetBoundingBox(const core::aabbox3df& box) override;

            //! sets a flag of all contained materials to a new value
            void SetMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue) override;

            //! set the hardware mapping hint, for driver
            void SetHardwareMappingHint(E_HARDWARE_MAPPING newMappingHint, E_BUFFER_TYPE buffer = EBT_VERTEX_AND_INDEX) override;

            //! flags the meshbuffer as changed, reloads hardware buffers
            void SetDirty(E_BUFFER_TYPE buffer = EBT_VERTEX_AND_INDEX) override;

            core::Array<IMeshBuffer *> mesh_buffer_;

            //! The bounding box of this mesh
            core::aabbox3d<f32> bounding_box_;
        };

        inline SMesh::~SMesh()
        {
            for (u32 i = 0; i < mesh_buffer_.Size(); i++)
            {
                delete mesh_buffer_[i];
            }
        }

        inline void SMesh::Clear()
        {
            mesh_buffer_.Clear();
        }

        inline u32 SMesh::GetMeshBufferCount() const
        {
            return mesh_buffer_.Size();
        }

        inline IMeshBuffer* SMesh::GetMeshBuffer(u32 nr) const
        {
            nr = core::min_(nr, mesh_buffer_.Size() - 1);
            return mesh_buffer_[nr];
        }

        inline IMeshBuffer* SMesh::GetMeshBuffer(const video::SMaterial& material) const
        {
            for (u32 i = 0; i < mesh_buffer_.Size(); i++)
            {
                if (material == mesh_buffer_[i]->GetMaterial())
                {
                    return mesh_buffer_[i];
                }
            }

            return nullptr;
        }

        inline void SMesh::RecalculateBoundingBox()//! recalculates the bounding box
        {
            if (mesh_buffer_.Size())
            {
                bounding_box_ = mesh_buffer_[0]->GetBoundingBox();
                for (u32 i = 1; i<mesh_buffer_.Size(); ++i)
                    bounding_box_.addInternalBox(mesh_buffer_[i]->GetBoundingBox());
            }
            else
                bounding_box_.reset(0.0f, 0.0f, 0.0f);
        }

        inline void SMesh::AddMeshBuffer(IMeshBuffer* mesh_buffer)
        {
            if (mesh_buffer != nullptr)
            {
                mesh_buffer_.PushBack(mesh_buffer);
            }
        }

        inline const core::aabbox3d<f32>& SMesh::GetBoundingBox() const
        {
            return bounding_box_;
        }

        inline void SMesh::SetBoundingBox(const core::aabbox3df& box)
        {
            bounding_box_ = box;
        }

        inline void SMesh::SetMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue)
        {
            //for (u32 i = 0; i<mesh_buffer_.Size(); ++i)
            //    mesh_buffer_[i]->GetMaterial().SetFlag(flag, newvalue);
        }

        inline void SMesh::SetHardwareMappingHint(E_HARDWARE_MAPPING newMappingHint, E_BUFFER_TYPE buffer)
        {
            //for (u32 i = 0; i<mesh_buffer_.Size(); ++i)
            //    mesh_buffer_[i]->SetHardwareMappingHint(newMappingHint, buffer);
        }

        //! flags the meshbuffer as changed, reloads hardware buffers
        inline void SMesh::SetDirty(E_BUFFER_TYPE buffer)
        {
            //for (u32 i = 0; i<mesh_buffer_.Size(); ++i)
            //    mesh_buffer_[i]->SetDirty(buffer);
        }
    }
}

#endif