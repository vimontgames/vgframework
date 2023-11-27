#include "core/Precomp.h"
#include "core/Scene/Scene.h"
#include "Universe.h"
#include "core/Object/AutoRegisterClass.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(Universe, "Universe");

    //--------------------------------------------------------------------------------------
    Universe::Universe(const string & _name, IObject * _parent) :
        IUniverse(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    Universe::~Universe()
    {
        RemoveAllScenes();
    }

    //--------------------------------------------------------------------------------------
    bool Universe::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    vector<Scene*> & Universe::getScenes()
    {
        return m_scenes;
    }

    //--------------------------------------------------------------------------------------
    bool Universe::SetActiveScene(IScene * _scene)
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
    core::IScene * Universe::GetActiveScene() const
    {
        return m_activeScene;
    }

    //--------------------------------------------------------------------------------------
    bool Universe::AddScene(IScene * _scene)
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
    bool Universe::RemoveScene(IScene * _scene)
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
    uint Universe::RemoveAllScenes()
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
    uint Universe::GetSceneCount() const
    {
        return m_scenes.count();
    }

    //--------------------------------------------------------------------------------------
    const IScene * Universe::GetScene(uint _index) const
    {
        return reinterpret_cast<const IScene*>(m_scenes[_index]);
    }

}