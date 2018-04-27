// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _IMESHBUFFER_H_
#define _IMESHBUFFER_H_

#include "KongTypes.h"
#include "SMaterial.h"
#include "S3DVertex.h"
#include "aabbox3d.h"

namespace kong
{
    namespace scene
    {
        class IMeshBuffer
        {
        public:
            virtual ~IMeshBuffer() = default;
            //! Get the material of this meshbuffer
            /** \return Material of this buffer. */
            virtual video::SMaterial& GetMaterial() = 0;

            //! Get the material of this meshbuffer
            /** \return Material of this buffer. */
            virtual const video::SMaterial& GetMaterial() const = 0;

            //! Get type of vertex data which is stored in this meshbuffer.
            /** \return Vertex type of this buffer. */
            virtual video::E_VERTEX_TYPE GetVertexType() const = 0;

            //! Get access to vertex data. The data is an array of vertices.
            /** Which vertex type is used can be determined by getVertexType().
            \return Pointer to array of vertices. */
            virtual const void* GetVertices() const = 0;

            //! Get access to vertex data. The data is an array of vertices.
            /** Which vertex type is used can be determined by getVertexType().
            \return Pointer to array of vertices. */
            virtual void* GetVertices() = 0;

            //! Get amount of vertices in meshbuffer.
            /** \return Number of vertices in this buffer. */
            virtual u32 GetVertexCount() const = 0;

            //! Get type of index data which is stored in this meshbuffer.
            /** \return Index type of this buffer. */
            virtual video::E_INDEX_TYPE GetIndexType() const = 0;

            //! Get access to Indices.
            /** \return Pointer to indices array. */
            virtual const u16* GetIndices() const = 0;

            //! Get access to Indices.
            /** \return Pointer to indices array. */
            virtual u16* GetIndices() = 0;

            //! Get amount of indices in this meshbuffer.
            /** \return Number of indices in this buffer. */
            virtual u32 GetIndexCount() const = 0;

            //! Get the currently used ID for identification of changes.
            /** This shouldn't be used for anything outside the VideoDriver. */
            virtual u32 GetVertexChangedID() const = 0;

            //! Get the currently used ID for identification of changes.
            /** This shouldn't be used for anything outside the VideoDriver. */
            virtual u32 GetIndexChangedID() const = 0;

            //! Get the axis aligned bounding box of this meshbuffer.
            /** \return Axis aligned bounding box of this buffer. */
            virtual const core::aabbox3df& GetBoundingBox() const = 0;

            //! Set axis aligned bounding box
            /** \param box User defined axis aligned bounding box to use
            for this buffer. */
            virtual void SetBoundingBox(const core::aabbox3df& box) = 0;

            //! Recalculates the bounding box. Should be called if the mesh changed.
            virtual void RecalculateBoundingBox() = 0;

            //! returns position of vertex i
            virtual const core::vector3df& GetPosition(u32 i) const = 0;

            //! returns position of vertex i
            virtual core::vector3df& GetPosition(u32 i) = 0;

            //! returns normal of vertex i
            virtual const core::vector3df& GetNormal(u32 i) const = 0;

            //! returns normal of vertex i
            virtual core::vector3df& GetNormal(u32 i) = 0;

            //! returns texture coord of vertex i
            virtual const core::vector2df& GetTCoords(u32 i) const = 0;

            //! returns texture coord of vertex i
            virtual core::vector2df& GetTCoords(u32 i) = 0;

            //! Append the vertices and indices to the current buffer
            /** Only works for compatible vertex types.
            \param vertices Pointer to a vertex array.
            \param numVertices Number of vertices in the array.
            \param indices Pointer to index array.
            \param numIndices Number of indices in array. */
            virtual void Append(const void* const vertices, u32 numVertices, const u16* const indices, u32 numIndices) = 0;
        };
    } // end namespace scene
} // end namespace kong

#endif