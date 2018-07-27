// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _CMESHSCENENODE_H_
#define _CMESHSCENENODE_H_
#include "IMesh.h"
#include "IMeshSceneNode.h"

namespace kong
{
    namespace scene
    {
        class CMeshSceneNode : public IMeshSceneNode
        {
        public:

            //! constructor
            CMeshSceneNode(IMesh* mesh, ISceneNode* parent, ISceneManager* mgr, s32 id,
                const core::vector3df& position = core::vector3df(0, 0, 0),
                const core::vector3df& rotation = core::vector3df(0, 0, 0),
                const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

            //! destructor
            virtual ~CMeshSceneNode();

            //! Set a new mesh to display
            void SetMesh(IMesh *mesh) override;

            IMesh *GetMesh() override;

            //! frame
            void OnRegisterSceneNode() override;

            void Render() override;

            const core::aabbox3d<f32>& GetBoundingBox() const override;

            void NormalizeVertice() override;

            //! normalize vertices to (1, 1, 1)
            void NormalizeVerticeToCube();

        protected:
            void CopyMaterials();

            core::Array<video::SMaterial> materials_;
            core::aabbox3d<f32> box_;
            video::SMaterial read_only_material_;

            IMesh* mesh_;

            s32 PassCount;
        };
    } // end namespace scene
} // end namespace kong

#endif