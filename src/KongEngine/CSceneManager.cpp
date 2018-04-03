#include "CSceneManager.h"

namespace kong
{
    namespace scene
    {
        CSceneManager::CSceneManager(video::IVideoDriver* driver)
            : ISceneNode(nullptr, nullptr), driver_(driver), shadow_color_(150, 0, 0, 0),
            ambient_light_(0, 0, 0, 0), active_camera_(nullptr)
        {
            // root node's scene manager
            scene_manager_ = this;


        }

        CSceneManager::~CSceneManager()
        {
        }

        IMeshSceneNode* CSceneManager::AddCubeSceneNode(f32 size, ISceneNode* parent, s32 id,
            const core::Vector3Df& position, const core::Vector3Df& rotation, const core::Vector3Df& scale)
        {

        }

        ICameraSceneNode* CSceneManager::AddCameraSceneNode(ISceneNode* parent, const core::Vector3Df& position,
            const core::Vector3Df& lookat, s32 id, bool make_active)
        {
        }

        ISceneNode* CSceneManager::AddSceneNode(const char* scene_node_name, ISceneNode* parent)
        {
        }

        void CSceneManager::SetShadowColor(video::SColor color)
        {
            shadow_color_ = color;
        }

        video::SColor CSceneManager::GetShadowColor() const
        {
            return shadow_color_;
        }

        void CSceneManager::SetActiveCamera(ICameraSceneNode* camera)
        {
            active_camera_ = camera;
        }

        void CSceneManager::Clear()
        {
            RemoveAll();
        }

        void CSceneManager::RemoveAll()
        {
            ISceneNode::RemoveAll();
            SetActiveCamera(nullptr);
            if (driver_ != nullptr)
            {
                driver_->SetMaterial(video::SMaterial());
            }
        }
    } // end namespace scene
} // end namespace kong
