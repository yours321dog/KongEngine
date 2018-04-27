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
        class CLODSceneNode : public ISceneNode
        {
        private:
            class Vertex;
            class Triangle;
            class Triangle {
            public:
                Vertex* vertex[3];
                Triangle(Vertex* v1, Vertex* v2, Vertex* v3);

                bool HasVertex(Vertex* v) const;
                void ReplaceVertex(Vertex* target, Vertex* v);
                bool CheckAndRemove(int num, core::Array<Triangle*> *list) const;
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


            IMesh* DefaultMesh;
            IMesh** LODMesh;
            IMesh* CurrentMesh;
            IMeshBuffer* test;
            u8 CurrentLevel;
            u8 LevelCount;
            f32 LODBegin;
            f32 LODLast;
            bool LODOn;
            core::Array<Vertex*> Verts[8];
            core::Array<Triangle*> Tris[8];
        public:

            CLODSceneNode(IMesh* mesh, scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id, u32 numOfCollapseOnLast,
                u8 numOfLevels = 4, f32 LODBeginDist = 10, f32 LODLastDist = 30, bool combineDuplicateVertices = true);


            void OnRegisterSceneNode() override;

            void Render() override;

            const core::aabbox3d<f32>& GetBoundingBox() const override;

            void SetLODOn(bool on);
            bool GetLODOn() const;

        };
    }
}


#endif