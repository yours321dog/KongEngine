#include "CLodSceneNode.h"
#include "CSceneManager.h"
#include "CMeshManipulator.h"

namespace kong
{
    namespace scene
    {
        CLODSceneNode::Triangle::Triangle(Vertex* v1, Vertex* v2, Vertex* v3)
        {
            vertex[0] = v1;
            vertex[1] = v2;
            vertex[2] = v3;
            for (u32 x = 0; x < 3; x++)
            {
                vertex[x]->face.PushBack(this);
                for (u32 y = 0; y < 3; y++) {
                    bool add = true;
                    for (u32 i = 0; i < vertex[x]->neighbors.Size(); i++)
                    {
                        if (vertex[y]->id == vertex[x]->neighbors[i]->id || y == x)
                        {
                            add = false;
                        }
                    }
                    if (add)
                    {
                        vertex[x]->neighbors.PushBack(vertex[y]);

                    }
                }
            }
        }

        bool CLODSceneNode::Triangle::HasVertex(Vertex* v) const
        {
            return (v->position == vertex[0]->position || v->position == vertex[1]->position || v->position == vertex[2]->position);
        }

        void CLODSceneNode::Triangle::ReplaceVertex(Vertex* target, Vertex* v)
        {
            for (auto& x : vertex)
            {
                if (x->position == target->position)
                {
                    x = v;
                }
            }
        }

        bool CLODSceneNode::Triangle::CheckAndRemove(int num, core::Array<Triangle*>* list) const
        {
            if (vertex[0]->position == vertex[1]->position || vertex[1]->position == vertex[2]->position || vertex[2]->position == vertex[0]->position) 
            {
                list->Erase(num);
                return true;
            }
            return false;
        }

        CLODSceneNode::Vertex::Vertex(): id(0)
        {
        }

        IMeshBuffer* CLODSceneNode::BuildMeshBufferFromTriangles(IMeshBuffer* oldmb, core::Array<Triangle*> arr)
        {
            SMeshBuffer* newm = new SMeshBuffer();
            newm->material_ = oldmb->GetMaterial();
            u16* indices = new u16[arr.Size() * 3];
            for (int x = 0; x < arr.Size(); x++) {
                for (int y = 0; y < 3; y++) {
                    indices[x * 3 + y] = arr[x]->vertex[y]->id;
                }
            }
            newm->Append(oldmb->GetVertices(), oldmb->GetVertexCount(), indices, arr.Size() * 3);
            return newm;
        }

        CLODSceneNode::CLODSceneNode(IMesh* mesh, scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id,
            u32 numOfCollapseOnLast, u8 numOfLevels, f32 LODBeginDist, f32 LODLastDist,
            bool combineDuplicateVertices)
            : scene::ISceneNode(parent, mgr, id), test(nullptr)
        {
            DefaultMesh = mesh;
            CurrentMesh = mesh;
            //CurrentMesh->getMeshBuffer(0)->getMaterial().Wireframe = true;
            LODBegin = LODBeginDist;
            LODLast = LODLastDist;
            LevelCount = numOfLevels;
            CurrentLevel = 0;
            LODMesh = new IMesh*[numOfLevels];
            LODOn = true;

            if (combineDuplicateVertices)
            {
                DefaultMesh = scene_manager_->GetMeshManipulator()->createMeshWelded(DefaultMesh);
            }


            for (u32 x = 0; x < DefaultMesh->GetMeshBufferCount(); x++)
            {
                video::S3DVertex* verts = static_cast<video::S3DVertex*>(DefaultMesh->GetMeshBuffer(x)->GetVertices());

                for (u32 y = 0; y < DefaultMesh->GetMeshBuffer(x)->GetVertexCount(); y++)
                {
                    Vertex* vert = new Vertex();
                    vert->position = verts[y].pos_;
                    vert->id = y;
                    Verts[x].PushBack(vert);
                }
            }

            for (u32 x = 0; x < DefaultMesh->GetMeshBufferCount(); x++)
            {
                u16* indices = DefaultMesh->GetMeshBuffer(x)->GetIndices();

                for (u32 y = 0; y < DefaultMesh->GetMeshBuffer(x)->GetIndexCount(); y += 3)
                {
                    Triangle* tri = new Triangle(Verts[x][indices[y]], Verts[x][indices[y + 1]],
                                                 Verts[x][indices[y + 2]]);

                    Tris[x].PushBack(tri);
                }
            }

            for (u32 x = 0; x < LevelCount; x++)
            {
                LODMesh[x] = DefaultMesh;
            }

            const u32 collapsePerLevel = numOfCollapseOnLast / (LevelCount - 1);
            const u32 collapsePerBuffer = collapsePerLevel / DefaultMesh->GetMeshBufferCount();


            for (u32 x = 1; x < LevelCount; x++)
            {
                SMesh* newLod = new SMesh();
                for (u32 y = 0; y < DefaultMesh->GetMeshBufferCount(); y++)
                {
                    for (u32 i = 0; i < collapsePerBuffer; i++)
                    {
                        const int vert = rand() % Verts[y].Size();
                        const int neighbor = rand() % Verts[y][vert]->neighbors.Size();
                        for (u32 u = 0; u < Tris[y].Size(); u++)
                        {
                            if (Tris[y][u]->HasVertex(Verts[y][vert]))
                            {
                                Tris[y][u]->ReplaceVertex(Verts[y][vert], Verts[y][vert]->neighbors[neighbor]);
                            }
                        }
                    }
                    SMeshBuffer* mn = new SMeshBuffer();
                    for (u32 xx = 0; xx < Tris[0].Size(); xx++)
                    {
                        if (Tris[0][xx]->CheckAndRemove(xx, &Tris[0]))
                        {
                            xx--;
                        }
                    }
                    mn = dynamic_cast<SMeshBuffer*>(BuildMeshBufferFromTriangles(DefaultMesh->GetMeshBuffer(y), Tris[y])
                    );
                    newLod->AddMeshBuffer(mn);
                    newLod->RecalculateBoundingBox();
                }
                LODMesh[x] = newLod;
            }
        }

        void CLODSceneNode::OnRegisterSceneNode()
        {
            if (IsVisible)
                scene_manager_->RegisterNodeForRendering(this, ESNRP_SOLID);

            ISceneNode::OnRegisterSceneNode();
        }

        void CLODSceneNode::Render()
        {
            if (LODOn) 
            {
                core::vector3df cameraPos = scene_manager_->GetActiveCamera()->GetPosition();
                f32 dist = absolute_tranform_.GetTranslation().GetDistanceFrom(cameraPos);
                CurrentLevel = 0;
                float increment = (LODLast - LODBegin) / LevelCount;
                for (u32 x = 0; x < LevelCount; x++) 
                {
                    if (dist >= (LODBegin + increment*x)*(LODBegin + increment*x))
                        CurrentLevel = x;
                }
                CurrentMesh = LODMesh[CurrentLevel];

            }
            else
            {
                CurrentMesh = DefaultMesh;
            }

            video::IVideoDriver* driver = scene_manager_->GetVideoDriver();
            driver->SetTransform(video::ETS_WORLD, absolute_tranform_);
            for (u32 x = 0; x < CurrentMesh->GetMeshBufferCount(); x++)
            {
                driver->SetMaterial(CurrentMesh->GetMeshBuffer(x)->GetMaterial());
                driver->DrawMeshBuffer(CurrentMesh->GetMeshBuffer(x));
            }
        }

        const core::aabbox3d<f32>& CLODSceneNode::GetBoundingBox() const
        {
            return CurrentMesh->GetBoundingBox();
        }

        void CLODSceneNode::SetLODOn(const bool on)
        {
            LODOn = on;
        }

        bool CLODSceneNode::GetLODOn() const
        {
            return LODOn;
        }
    }
}