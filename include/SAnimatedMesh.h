// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_ANIMATED_MESH_H_INCLUDED__
#define __S_ANIMATED_MESH_H_INCLUDED__

#include "IAnimatedMesh.h"
#include "IMesh.h"
#include "aabbox3d.h"
#include "Array.h"

namespace kong
{
    namespace scene
    {

        //! Simple implementation of the IAnimatedMesh interface.
        struct SAnimatedMesh : public IAnimatedMesh
        {
            //! constructor
            SAnimatedMesh(scene::IMesh* mesh = 0, scene::E_ANIMATED_MESH_TYPE type = scene::EAMT_UNKNOWN) : IAnimatedMesh(), FramesPerSecond(25.f), Type(type)
            {
#ifdef _DEBUG
                //setDebugName("SAnimatedMesh");
#endif
                addMesh(mesh);
                RecalculateBoundingBox();
            }

            //! destructor
            virtual ~SAnimatedMesh()
            {
                // drop meshes
                for (u32 i = 0; i < Meshes.Size(); ++i)
                {
                    delete Meshes[i];
                }
            }

            //! Gets the frame count of the animated mesh.
            /** \return Amount of frames. If the amount is 1, it is a static, non animated mesh. */
            virtual u32 getFrameCount() const
            {
                return Meshes.Size();
            }

            //! Gets the default animation speed of the animated mesh.
            /** \return Amount of frames per second. If the amount is 0, it is a static, non animated mesh. */
            virtual f32 getAnimationSpeed() const
            {
                return FramesPerSecond;
            }

            //! Gets the frame count of the animated mesh.
            /** \param fps Frames per second to play the animation with. If the amount is 0, it is not animated.
            The actual speed is set in the scene node the mesh is instantiated in.*/
            virtual void setAnimationSpeed(f32 fps)
            {
                FramesPerSecond = fps;
            }

            //! Returns the IMesh interface for a frame.
            /** \param frame: Frame number as zero based index. The maximum frame number is
            getFrameCount() - 1;
            \param detailLevel: Level of detail. 0 is the lowest,
            255 the highest level of detail. Most meshes will ignore the detail level.
            \param startFrameLoop: start frame
            \param endFrameLoop: end frame
            \return The animated mesh based on a detail level. */
            virtual IMesh* getMesh(s32 frame, s32 detailLevel = 255, s32 startFrameLoop = -1, s32 endFrameLoop = -1)
            {
                if (Meshes.Empty())
                    return nullptr;

                return Meshes[frame];
            }

            //! adds a Mesh
            void addMesh(IMesh* mesh)
            {
                if (mesh)
                {
                    //mesh->grab();
                    Meshes.PushBack(mesh);
                }
            }

            //! Returns an axis aligned bounding box of the mesh.
            /** \return A bounding box of this mesh is returned. */
            virtual const core::aabbox3d<f32>& GetBoundingBox() const
            {
                return Box;
            }

            //! set user axis aligned bounding box
            virtual void SetBoundingBox(const core::aabbox3df& box)
            {
                Box = box;
            }

            //! Recalculates the bounding box.
            void RecalculateBoundingBox()
            {
                Box.reset(0, 0, 0);

                if (Meshes.Empty())
                    return;

                for (u32 i = 0; i < Meshes.Size(); ++i)
                    Meshes[i]->RecalculateBoundingBox();

                Box = Meshes[0]->GetBoundingBox();

                for (u32 i = 1; i<Meshes.Size(); ++i)
                    Box.addInternalBox(Meshes[i]->GetBoundingBox());
            }

            //! Returns the type of the animated mesh.
            virtual E_ANIMATED_MESH_TYPE GetMeshType() const
            {
                return Type;
            }

            //! returns amount of mesh buffers.
            virtual u32 GetMeshBufferCount() const
            {
                if (Meshes.Empty())
                    return 0;

                return Meshes[0]->GetMeshBufferCount();
            }

            //! returns pointer to a mesh buffer
            virtual IMeshBuffer* GetMeshBuffer(u32 nr) const
            {
                if (Meshes.Empty())
                    return nullptr;

                return Meshes[0]->GetMeshBuffer(nr);
            }

            //! Returns pointer to a mesh buffer which fits a material
            /** \param material: material to search for
            \return Returns the pointer to the mesh buffer or
            NULL if there is no such mesh buffer. */
            virtual IMeshBuffer* GetMeshBuffer(const video::SMaterial &material) const
            {
                if (Meshes.Empty())
                    return nullptr;

                return Meshes[0]->GetMeshBuffer(material);
            }

            //! Set a material flag for all meshbuffers of this mesh.
            virtual void SetMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue)
            {
                for (u32 i = 0; i<Meshes.Size(); ++i)
                    Meshes[i]->SetMaterialFlag(flag, newvalue);
            }

            //! set the hardware mapping hint, for driver
            virtual void SetHardwareMappingHint(E_HARDWARE_MAPPING newMappingHint, E_BUFFER_TYPE buffer = EBT_VERTEX_AND_INDEX)
            {
                for (u32 i = 0; i<Meshes.Size(); ++i)
                    Meshes[i]->SetHardwareMappingHint(newMappingHint, buffer);
            }

            //! flags the meshbuffer as changed, reloads hardware buffers
            virtual void SetDirty(E_BUFFER_TYPE buffer = EBT_VERTEX_AND_INDEX)
            {
                for (u32 i = 0; i<Meshes.Size(); ++i)
                    Meshes[i]->SetDirty(buffer);
            }

            //! All meshes defining the animated mesh
            core::Array<IMesh*> Meshes;

            //! The bounding box of this mesh
            core::aabbox3d<f32> Box;

            //! Default animation speed of this mesh.
            f32 FramesPerSecond;

            //! The type of the mesh.
            E_ANIMATED_MESH_TYPE Type;
        };


    } // end namespace scene
} // end namespace irr

#endif

