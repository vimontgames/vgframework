#include "engine/Precomp.h"
#include "engine/World/Scene/Scene.h"
#include "World.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(World, "World");

    //--------------------------------------------------------------------------------------
    World::World(const string & _name, IObject * _parent) :
        IWorld(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    World::~World()
    {
        RemoveAllScenes();
    }

    //--------------------------------------------------------------------------------------
    bool World::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    vector<Scene*> & World::getScenes()
    {
        return m_scenes;
    }

    //--------------------------------------------------------------------------------------
    bool World::SetActiveScene(IScene * _scene)
    {
        if (_scene != m_activeScene)
        {
            if (m_scenes.exists((Scene*)_scene))
            {
                m_activeScene = (Scene*)_scene;
                return true;
            }
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    core::IScene * World::GetActiveScene() const
    {
        return m_activeScene;
    }

    //--------------------------------------------------------------------------------------
    bool World::AddScene(IScene * _scene)
    {
        if (nullptr != _scene && !m_scenes.exists((Scene*)_scene))
        {
            _scene->setParent(this);
            _scene->addRef();
            m_scenes.push_back((Scene*)_scene);

            if (nullptr == m_activeScene)
                SetActiveScene(_scene);

            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool World::RemoveScene(IScene * _scene)
    {
        if (nullptr != _scene && m_scenes.exists((Scene*)_scene))
        {
            m_scenes.remove((Scene*)_scene);

            if (m_activeScene == _scene)
                SetActiveScene(nullptr);

            _scene->release();
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    uint World::RemoveAllScenes()
    {
        const uint sceneCount = m_scenes.count();
        for (uint i = 0; i < sceneCount; ++i)
        {
            VG_SAFE_RELEASE(m_scenes[i]);
        }
        m_scenes.clear();
        return sceneCount;
    }

    //--------------------------------------------------------------------------------------
    uint World::GetSceneCount() const
    {
        return m_scenes.count();
    }

    //--------------------------------------------------------------------------------------
    const IScene * World::GetScene(uint _index) const
    {
        return reinterpret_cast<const IScene*>(m_scenes[_index]);
    }

}