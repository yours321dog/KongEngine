// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _CMESHBUFFER_H_
#define _CMESHBUFFER_H_

#include "IMeshBuffer.h"
#include "Array.h"

namespace kong
{
    namespace scene
    {
        template <class T>
        class CMeshBuffer : public IMeshBuffer
        {
            CMeshBuffer();

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

            video::SMaterial material_;
            core::Array<T>  vertices_;
            core::Array<u16> indices_;

            u32 changed_id_vertex_;
            u32 changed_id_index;

            // Bouding box of this meshbuffer
            //core::aabbox3d<f32> BoundingBox;
        };

        template <class T>
        CMeshBuffer<T>::CMeshBuffer() 
            : changed_id_vertex_(1), changed_id_index(1)
        {

        }

        template <class T>
        video::SMaterial& CMeshBuffer<T>::GetMaterial()
        {
            return material_;
        }

        template <class T>
        const video::SMaterial& CMeshBuffer<T>::GetMaterial() const
        {
            return material_;
        }

        template <class T>
        const void* CMeshBuffer<T>::GetVertices() const
        {
            return vertices_.ConstPointer();
        }

        template <class T>
        void* CMeshBuffer<T>::GetVertices()
        {
            return vertices_.Pointer();
        }

        template <class T>
        u32 CMeshBuffer<T>::GetVertexCount() const
        {
            return vertices_.Size();
        }

        template <class T>
        const u16* CMeshBuffer<T>::GetIndices() const
        {
            return indices_.ConstPointer();
        }

        template <class T>
        u16* CMeshBuffer<T>::GetIndices()
        {
            return indices_.Pointer();
        }

        template <class T>
        u32 CMeshBuffer<T>::GetIndexCount() const
        {
            return indices_.Size();
        }
    } // end namespace scene
} // end namespace kong

#endif