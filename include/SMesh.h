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

            void AddMeshBuffer(IMeshBuffer *mesh_buffer);

            core::Array<IMeshBuffer *> mesh_buffer_;
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

        inline void SMesh::AddMeshBuffer(IMeshBuffer* mesh_buffer)
        {
            if (mesh_buffer != nullptr)
            {
                mesh_buffer_.PushBack(mesh_buffer);
            }
        }
    }
}

#endif