// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _CPLANESCENENODE_H_
#define _CPLANESCENENODE_H_

#include "IMeshSceneNode.h"

namespace kong
{
    namespace scene
    {
        class CPlaneSceneNode : public IMeshSceneNode
        {
        public:
            //! constructor
            CPlaneSceneNode(f32 size, ISceneNode *parent, ISceneManager *mgr, s32 id,
                const core::Vector3Df &position = core::Vector3Df(0.f, 0.f, 0.f),
                const core::Vector3Df &rotation = core::Vector3Df(0.f, 0.f, 0.f),
                const core::Vector3Df &scale = core::Vector3Df(1.f, 1.f, 1.f));

            virtual ~CPlaneSceneNode();

            //! returns the axis aligned bounding box of this node
            const core::aabbox3d<f32>& GetBoundingBox() const override;

            //! render the node.
            void Render() override;

            video::SMaterial& GetMaterial(u32 num) override;

            u32 GetMaterialCount() const override;

            //! Set a new mesh to display
            void SetMesh(IMesh *mesh) override;

            IMesh *GetMesh() override;

            // register node
            void OnRegisterSceneNode() override;

        private:
            void GenerateMesh();

            f32 size_;

            IMesh *mesh_{};
        };
    }
}

#endif