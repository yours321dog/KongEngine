// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _IMESHBUFFER_H_
#define _IMESHBUFFER_H_

#include "KongTypes.h"
#include "SMaterial.h"
#include "S3DVertex.h"

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

            ////! Get type of vertex data which is stored in this meshbuffer.
            ///** \return Vertex type of this buffer. */
            //virtual video::E_VERTEX_TYPE getVertexType() const = 0;

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
        };
    } // end namespace scene
} // end namespace kong

#endif