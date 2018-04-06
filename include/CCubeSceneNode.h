// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _CCUBESCENENODE_H_
#define _CCUBESCENENODE_H_

#include "IMeshSceneNode.h"

namespace kong
{
    namespace scene
    {
        class CCubeSceneNode : public IMeshSceneNode
        {
        public:
            //! constructor
            CCubeSceneNode(f32 size, ISceneNode *parent, ISceneManager *mgr, s32 id,
                const core::Vector3Df &position = core::Vector3Df(0.f, 0.f, 0.f),
                const core::Vector3Df &rotation = core::Vector3Df(0.f, 0.f, 0.f),
                const core::Vector3Df &scale = core::Vector3Df(1.f, 1.f, 1.f));

            virtual ~CCubeSceneNode();

            //! render the node.
            void Render() override;

            video::SMaterial& GetMaterial(u32 num) override;

            u32 GetMaterialCount() const override;

            //! Set a new mesh to display
            void SetMesh(IMesh *mesh) override;

            IMesh *GetMesh() override;

        private:
            void GenerateMesh();

            f32 size_;

            IMesh *mesh_;
        };
    }
}

#endif