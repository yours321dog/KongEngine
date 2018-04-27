#ifndef _CLODSCENENODE_H_
#define _CLODSCENENODE_H_
#include "ISceneNode.h"
#include "IMeshBuffer.h"
#include "CMeshBuffer.h"
#include "IMesh.h"
#include "SMesh.h"
#include "IVideoDriver.h"

namespace kong
{
    namespace scene
    {
        class CLodSceneNode : public ISceneNode
        {

        public:

            CLodSceneNode(IMesh* mesh, scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id, u32 numOfCollapseOnLast,
                u8 numOfLevels = 4, f32 LODBeginDist = 10, f32 LODLastDist = 30, bool combineDuplicateVertices = true);

            virtual ~CLodSceneNode();

            void OnRegisterSceneNode() override;

            void Render() override;

            const core::aabbox3d<f32>& GetBoundingBox() const override;

            void SetLODOn(bool on);
            bool GetLODOn() const;

        private:
            class Vertex;
            class Triangle;
            class Triangle {
            public:
                Vertex* vertex[3];
                Triangle(Vertex* v1, Vertex* v2, Vertex* v3);

                bool HasVertex(Vertex* v) const;
                void ReplaceVertex(Vertex* target, Vertex* v);
                bool CheckAndRemove(u32 num, core::Array<Triangle*> *list) const;
            };

            class Vertex
            {
            public:
                Vertex();
                core::Vector3Df position;
                core::Array<Vertex*> neighbors;
                core::Array<Triangle*> face;
                int id;
            };


            static IMeshBuffer* BuildMeshBufferFromTriangles(IMeshBuffer* oldmb, core::Array<Triangle*> arr);


            IMesh* default_mesh_;
            IMesh** lod_mesh_;
            IMesh* current_mesh_;
            IMeshBuffer* test;
            u8 current_level_;
            u8 level_count_;
            f32 lod_begin_;
            f32 lod_last_;
            bool lod_on_;
            core::Array<Vertex*> verts_[8];
            core::Array<Triangle*> triangles_[8];

        };
    }
}


#endif