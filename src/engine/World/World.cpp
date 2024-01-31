#include "engine/Precomp.h"
#include "engine/World/Scene/Scene.h"
#include "World.h"

#include "GameObjectHierarchy.hpp"
#include "Scene/Scene.hpp"
#include "Prefab/Prefab.hpp"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(World, "World");

    //--------------------------------------------------------------------------------------
    World::World(const string & _name, IObject * _parent) :
        IWorld(_name, _parent)
    {
        for (uint j = 0; j < enumCount<SceneType>(); ++j)
            m_activeScene[j] = nullptr;
    }

    //--------------------------------------------------------------------------------------
    World::~World()
    {
        for (uint j = 0; j < enumCount<SceneType>(); ++j)
        {
            auto sceneType = (SceneType)j;
            RemoveAllScenes(sceneType);
        }
    }

    //--------------------------------------------------------------------------------------
    bool World::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    vector<Scene*> & World::getScenes(SceneType _sceneType)
    {
        return m_scenes[asInteger(_sceneType)];
    }

    //--------------------------------------------------------------------------------------
    bool World::SetActiveScene(IBaseScene * _scene, SceneType _sceneType)
    {
        const auto typeIndex = asInteger(_sceneType);

        if (_scene != m_activeScene[typeIndex])
        {
            if (m_scenes[typeIndex].exists((Scene *)_scene))
            {
                m_activeScene[typeIndex] = (Scene *)_scene;
                return true;
            }
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    core::IBaseScene * World::GetActiveScene(SceneType _sceneType) const
    {
        const auto typeIndex = asInteger(_sceneType);
        return m_activeScene[typeIndex];
    }

    //--------------------------------------------------------------------------------------
    bool World::AddScene(IBaseScene * _scene, SceneType _sceneType)
    {
        const auto typeIndex = asInteger(_sceneType);
        if (nullptr != _scene && !m_scenes[typeIndex].exists((Scene *)_scene))
        {
            _scene->setParent(this);
            _scene->addRef();
            m_scenes[typeIndex].push_back((Scene*)_scene);

            if (nullptr == m_activeScene)
                SetActiveScene(_scene, _sceneType);

            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool World::RemoveScene(IBaseScene * _scene, SceneType _sceneType)
    {
        const auto typeIndex = asInteger(_sceneType);
        if (nullptr != _scene && m_scenes[typeIndex].exists((Scene*)_scene))
        {
            m_scenes[typeIndex].remove((Scene*)_scene);

            if (m_activeScene[typeIndex] == _scene)
                SetActiveScene(nullptr, _sceneType);

            _scene->release();
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    uint World::RemoveAllScenes(SceneType _sceneType)
    {
        const auto typeIndex = asInteger(_sceneType);
        const uint sceneCount = m_scenes[typeIndex].count();
        for (uint i = 0; i < sceneCount; ++i)
        {
            VG_SAFE_RELEASE(m_scenes[typeIndex][i]);
        }
        m_scenes[typeIndex].clear();
        return sceneCount;
    }

    //--------------------------------------------------------------------------------------
    uint World::GetSceneCount(SceneType _sceneType) const
    {
        const auto typeIndex = asInteger(_sceneType);
        return m_scenes[typeIndex].count();
    }

    //--------------------------------------------------------------------------------------
    const IBaseScene * World::GetScene(uint _index, SceneType _sceneType) const
    {
        const auto typeIndex = asInteger(_sceneType);
        return reinterpret_cast<const IBaseScene*>(m_scenes[typeIndex][_index]);
    }

}