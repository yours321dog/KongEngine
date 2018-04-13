// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _CMESHBUFFER_H_
#define _CMESHBUFFER_H_

#include "IMeshBuffer.h"
#include "Array.h"
#include "aabbox3d.h"

namespace kong
{
    namespace scene
    {
        template <class T>
        class CMeshBuffer : public IMeshBuffer
        {
        public:
            CMeshBuffer();

            virtual ~CMeshBuffer() = default;

            //! Get the material of this meshbuffer
            /** \return Material of this buffer. */
            virtual video::SMaterial& GetMaterial();

            //! Get the material of this meshbuffer
            /** \return Material of this buffer. */
            virtual const video::SMaterial& GetMaterial() const;

            //! Get type of vertex data which is stored in this meshbuffer.
            /** \return Vertex type of this buffer. */
            virtual video::E_VERTEX_TYPE GetVertexType() const;

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

            //! Get type of index data which is stored in this meshbuffer.
            virtual video::E_INDEX_TYPE GetIndexType() const;

            //! Get access to Indices.
            /** \return Pointer to indices array. */
            virtual const u16* GetIndices() const;

            //! Get access to Indices.
            /** \return Pointer to indices array. */
            virtual u16* GetIndices();

            //! Get amount of indices in this meshbuffer.
            /** \return Number of indices in this buffer. */
            virtual u32 GetIndexCount() const;

            //! Get the currently used ID for identification of changes.
            /** This shouldn't be used for anything outside the VideoDriver. */
            virtual u32 GetVertexChangedID() const;

            //! Get the currently used ID for identification of changes.
            /** This shouldn't be used for anything outside the VideoDriver. */
            virtual u32 GetIndexChangedID() const;

            //! Get the axis aligned bounding box
            /** \return Axis aligned bounding box of this buffer. */
            virtual const core::aabbox3df& GetBoundingBox() const;

            //! Set the axis aligned bounding box
            /** \param box New axis aligned bounding box for this buffer. */
            //! set user axis aligned bounding box
            virtual void SetBoundingBox(const core::aabbox3df& box);


            //! Recalculate the bounding box.
            /** should be called if the mesh changed. */
            virtual void RecalculateBoundingBox();

            //! returns position of vertex i
            virtual const core::vector3df& GetPosition(u32 i) const;

            //! returns position of vertex i
            virtual core::vector3df& GetPosition(u32 i);

            //! returns normal of vertex i
            virtual const core::vector3df& GetNormal(u32 i) const;

            //! returns normal of vertex i
            virtual core::vector3df& GetNormal(u32 i);

            //! returns texture coord of vertex i
            virtual const core::vector2df& GetTCoords(u32 i) const;

            //! returns texture coord of vertex i
            virtual core::vector2df& GetTCoords(u32 i);

            video::SMaterial material_;
            core::Array<T>  vertices_;
            core::Array<u16> indices_;
            //! Bounding box of this meshbuffer.
            core::aabbox3d<f32> bounding_box_;

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
        video::E_VERTEX_TYPE CMeshBuffer<T>::GetVertexType() const
        {
            return T().GetType();
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
        video::E_INDEX_TYPE CMeshBuffer<T>::GetIndexType() const
        {
            return video::EIT_16BIT;
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

        template <class T>
        u32 CMeshBuffer<T>::GetVertexChangedID() const
        {
            return changed_id_vertex_;
        }

        template <class T>
        u32 CMeshBuffer<T>::GetIndexChangedID() const
        {
            return changed_id_index;
        }

        template <class T>
        const core::aabbox3d<f32>& CMeshBuffer<T>::GetBoundingBox() const
        {
            return bounding_box_;
        }


        template <class T>
        void CMeshBuffer<T>::SetBoundingBox(const core::aabbox3df& box)
        {
            bounding_box_ = box;
        }


        template <class T>
        void CMeshBuffer<T>::RecalculateBoundingBox()
        {
            if (vertices_.Empty())
                bounding_box_.reset(0, 0, 0);
            else
            {
                bounding_box_.reset(vertices_[0].pos_);
                for (u32 i = 1; i<vertices_.Size(); ++i)
                    bounding_box_.addInternalPoint(vertices_[i].pos_);
            }
        }

        template <class T>
        const core::vector3df& CMeshBuffer<T>::GetPosition(u32 i) const
        {
            return vertices_[i].pos_;
        }

        template <class T>
        core::vector3df& CMeshBuffer<T>::GetPosition(u32 i)
        {
            return vertices_[i].pos_;
        }

        template <class T>
        const core::vector3df& CMeshBuffer<T>::GetNormal(u32 i) const
        {
            return vertices_[i].normal_;
        }

        template <class T>
        core::vector3df& CMeshBuffer<T>::GetNormal(u32 i)
        {
            return vertices_[i].normal_;
        }

        template <class T>
        const core::vector2df& CMeshBuffer<T>::GetTCoords(u32 i) const
        {
            return vertices_[i].texcoord_;
        }

        template <class T>
        core::vector2df& CMeshBuffer<T>::GetTCoords(u32 i)
        {
            return vertices_[i].texcoord_;
        }

        //! Standard meshbuffer
        typedef CMeshBuffer<video::S3DVertex> SMeshBuffer;
        //! Meshbuffer with two texture coords per vertex, e.g. for lightmaps
        typedef CMeshBuffer<video::S3DVertex2TCoords> SMeshBufferLightMap;
        //! Meshbuffer with vertices having tangents stored, e.g. for normal mapping
        typedef CMeshBuffer<video::S3DVertexTangents> SMeshBufferTangents;
    } // end namespace scene
} // end namespace kong

#endif