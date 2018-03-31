// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _CMESHBUFFER_H_
#define _CMESHBUFFER_H_

#include "IMeshBuffer.h"

namespace kong
{
    namespace scene
    {
        class CMeshBuffer : public IMeshBuffer
        {
            //! Get the material of this meshbuffer
            /** \return Material of this buffer. */
            virtual video::SMaterial& GetMaterial();

            //! Get the material of this meshbuffer
            /** \return Material of this buffer. */
            virtual const video::SMaterial& GetMaterial() const;

            ////! Get type of vertex data which is stored in this meshbuffer.
            ///** \return Vertex type of this buffer. */
            //virtual video::E_VERTEX_TYPE getVertexType() const = 0;

            //! Get access to vertex data. The data is an array of vertices.
            /** Which vertex type is used can be determined by getVertexType().
            \return Pointer to array of vertices. */
            virtual const void* GetVertices() const;

            //! Get access to vertex data. The data is an array of vertices.
            /** Which vertex type is used can be determined by getVertexType().
            \return Pointer to array of vertices. */
            virtual void* GetVertices();

            //! Get amount of vertices in meshbuffer.
            /** \return Number of vertices in this buffer. */
            virtual u32 GetVertexCount() const;

            //! Get access to Indices.
            /** \return Pointer to indices array. */
            virtual const u16* GetIndices() const;

            //! Get access to Indices.
            /** \return Pointer to indices array. */
            virtual u16* GetIndices();

            //! Get amount of indices in this meshbuffer.
            /** \return Number of indices in this buffer. */
            virtual u32 GetIndexCount() const;
        };
    } // end namespace scene
} // end namespace kong

#endif