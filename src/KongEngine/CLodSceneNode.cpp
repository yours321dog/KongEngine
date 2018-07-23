#include "CLodSceneNode.h"
#include "CSceneManager.h"
#include "CMeshManipulator.h"

namespace kong
{
    namespace scene
    {
        CLodSceneNode::Triangle::Triangle(Vertex* v1, Vertex* v2, Vertex* v3)
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

        bool CLodSceneNode::Triangle::HasVertex(Vertex* v) const
        {
            return (v->position == vertex[0]->position || v->position == vertex[1]->position || v->position == vertex[2]->position);
        }

        void CLodSceneNode::Triangle::ReplaceVertex(Vertex* target, Vertex* v)
        {
            for (auto& x : vertex)
            {
                if (x->position == target->position)
                {
                    x = v;
                }
            }
        }

        bool CLodSceneNode::Triangle::CheckAndRemove(u32 num, core::Array<Triangle*>* list) const
        {
            if (num >= list->Size())
            {
                return false;
            }

            if (vertex[0]->position == vertex[1]->position || vertex[1]->position == vertex[2]->position || vertex[2]->position == vertex[0]->position) 
            {
                list->Erase(num);
                return true;
            }
            return false;
        }

        CLodSceneNode::Vertex::Vertex(): id(0)
        {
        }

        IMeshBuffer* CLodSceneNode::BuildMeshBufferFromTriangles(IMeshBuffer* oldmb, core::Array<Triangle*> arr)
        {
            SMeshBuffer* newm = new SMeshBuffer();
            newm->material_ = oldmb->GetMaterial();
            u16* indices = new u16[arr.Size() * 3];
            for (u32 x = 0; x < arr.Size(); x++)
            {
                for (u32 y = 0; y < 3; y++) 
                {
                    indices[x * 3 + y] = arr[x]->vertex[y]->id;
                }
            }
            newm->Append(oldmb->GetVertices(), oldmb->GetVertexCount(), indices, arr.Size() * 3);
            return newm;
        }

        CLodSceneNode::CLodSceneNode(IMesh* mesh, scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id,
            u32 numOfCollapseOnLast, u8 numOfLevels, f32 LODBeginDist, f32 LODLastDist,
            bool combineDuplicateVertices)
            : scene::ISceneNode(parent, mgr, id), test(nullptr)
        {
            default_mesh_ = mesh;
            current_mesh_ = mesh;
            //CurrentMesh->getMeshBuffer(0)->getMaterial().Wireframe = true;
            lod_begin_ = LODBeginDist;
            lod_last_ = LODLastDist;
            level_count_ = numOfLevels;
            current_level_ = 0;
            lod_mesh_ = new IMesh*[numOfLevels];
            lod_on_ = true;

            if (combineDuplicateVertices)
            {
                default_mesh_ = scene_manager_->GetMeshManipulator()->createMeshWelded(default_mesh_);
            }


            for (u32 x = 0; x < default_mesh_->GetMeshBufferCount(); x++)
            {
                video::S3DVertex* verts = static_cast<video::S3DVertex*>(default_mesh_->GetMeshBuffer(x)->GetVertices());

                for (u32 y = 0; y < default_mesh_->GetMeshBuffer(x)->GetVertexCount(); y++)
                {
                    Vertex* vert = new Vertex();
                    vert->position = verts[y].pos_;
                    vert->id = y;
                    verts_[x].PushBack(vert);
                }
            }

            for (u32 x = 0; x < default_mesh_->GetMeshBufferCount(); x++)
            {
                u16* indices = default_mesh_->GetMeshBuffer(x)->GetIndices();

                for (u32 y = 0; y < default_mesh_->GetMeshBuffer(x)->GetIndexCount(); y += 3)
                {
                    Triangle* tri = new Triangle(verts_[x][indices[y]], verts_[x][indices[y + 1]],
                                                 verts_[x][indices[y + 2]]);

                    triangles_[x].PushBack(tri);
                }
            }

            for (u32 x = 0; x < level_count_; x++)
            {
                lod_mesh_[x] = default_mesh_;
            }

            const u32 collapsePerLevel = numOfCollapseOnLast / (level_count_ - 1);
            const u32 collapsePerBuffer = collapsePerLevel / default_mesh_->GetMeshBufferCount();


            for (u32 x = 1; x < level_count_; x++)
            {
                SMesh* newLod = new SMesh();
                for (u32 y = 0; y < default_mesh_->GetMeshBufferCount(); y++)
                {
                    for (u32 i = 0; i < collapsePerBuffer; i++)
                    {
                        const int vert = rand() % verts_[y].Size();
                        const int neighbor = rand() % verts_[y][vert]->neighbors.Size();
                        for (u32 u = 0; u < triangles_[y].Size(); u++)
                        {
                            if (triangles_[y][u]->HasVertex(verts_[y][vert]))
                            {
                                triangles_[y][u]->ReplaceVertex(verts_[y][vert], verts_[y][vert]->neighbors[neighbor]);
                            }
                        }
                    }
                    SMeshBuffer* mn = new SMeshBuffer();
                    for (u32 xx = 0; xx < triangles_[0].Size(); xx++)
                    {
                        if (triangles_[0][xx]->CheckAndRemove(xx, &triangles_[0]))
                        {
                            xx--;
                        }
                    }
                    mn = dynamic_cast<SMeshBuffer*>(BuildMeshBufferFromTriangles(default_mesh_->GetMeshBuffer(y), triangles_[y]));
                    newLod->AddMeshBuffer(mn);
                    newLod->RecalculateBoundingBox();
                }
                lod_mesh_[x] = newLod;
            }
        }

        CLodSceneNode::~CLodSceneNode()
        {
            for (u32 i = 0; i < level_count_; i++)
            {
                delete lod_mesh_[i];
            }
            delete[] lod_mesh_;

            delete default_mesh_;

            for (u32 i = 0; i < 8; i++)
            {
                for (u32 j = 0; j < verts_[i].Size(); j++)
                {
                    delete verts_[i][j];
                }
            }

            for (u32 i = 0; i < 8; i++)
            {
                for (u32 j = 0; j < triangles_[i].Size(); j++)
                {
                    delete triangles_[i][j];
                }
            }
        }

        void CLodSceneNode::OnRegisterSceneNode()
        {
            if (is_visible_)
                scene_manager_->RegisterNodeForRendering(this, ESNRP_SOLID);

            ISceneNode::OnRegisterSceneNode();
        }

        void CLodSceneNode::Render()
        {
            if (lod_on_) 
            {
                core::vector3df cameraPos = scene_manager_->GetActiveCamera()->GetPosition();
                f32 dist = absolute_tranform_.GetTranslation().GetDistanceFrom(cameraPos);
                current_level_ = 0;
                float increment = (lod_last_ - lod_begin_) / level_count_;
                for (u32 x = 0; x < level_count_; x++) 
                {
                    if (dist >= (lod_begin_ + increment*x)*(lod_begin_ + increment*x))
                        current_level_ = x;
                }
                current_mesh_ = lod_mesh_[current_level_];

            }
            else
            {
                current_mesh_ = default_mesh_;
            }

            video::IVideoDriver* driver = scene_manager_->GetVideoDriver();
            driver->SetTransform(video::ETS_WORLD, absolute_tranform_);
            for (u32 x = 0; x < current_mesh_->GetMeshBufferCount(); x++)
            {
                driver->SetMaterial(current_mesh_->GetMeshBuffer(x)->GetMaterial());
                driver->DrawMeshBuffer(current_mesh_->GetMeshBuffer(x));
            }
        }

        const core::aabbox3d<f32>& CLodSceneNode::GetBoundingBox() const
        {
            return current_mesh_->GetBoundingBox();
        }

        void CLodSceneNode::SetLODOn(const bool on)
        {
            lod_on_ = on;
        }

        bool CLodSceneNode::GetLODOn() const
        {
            return lod_on_;
        }
    }
}