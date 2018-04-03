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
    } // end namespace scene
} // end namespace kong
