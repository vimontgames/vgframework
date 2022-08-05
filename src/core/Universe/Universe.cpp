#include "core/Precomp.h"
#include "core/Scene/Scene.h"
#include "Universe.h"
#include "core/Object/AutoRegisterClass.h"

namespace vg::core
{
    VG_AUTO_REGISTER_CLASS(Universe);

    //--------------------------------------------------------------------------------------
    Universe::Universe(const string & _name, IObject * _parent) :
        IUniverse(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    Universe::~Universe()
    {
        removeAllScenes();
    }

    //--------------------------------------------------------------------------------------
    bool Universe::registerClass(IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(Universe, "Universe", IClassDesc::Flags::None))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Universe::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Universe::setActiveScene(IScene * _scene)
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
    core::IScene * Universe::getActiveScene() const
    {
        return m_activeScene;
    }

    //--------------------------------------------------------------------------------------
    bool Universe::addScene(IScene * _scene)
    {
        if (nullptr != _scene && !m_scenes.exists((Scene*)_scene))
        {
            _scene->addRef();
            m_scenes.push_back((Scene*)_scene);

            if (nullptr == m_activeScene)
                setActiveScene(_scene);

            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Universe::removeScene(IScene * _scene)
    {
        if (nullptr != _scene && m_scenes.exists((Scene*)_scene))
        {
            m_scenes.remove((Scene*)_scene);

            if (m_activeScene == _scene)
                setActiveScene(nullptr);

            _scene->release();
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    uint Universe::removeAllScenes()
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
    uint Universe::getSceneCount() const
    {
        return m_scenes.count();
    }

    //--------------------------------------------------------------------------------------
    const IScene * Universe::getScene(uint _index) const
    {
        return reinterpret_cast<const IScene*>(m_scenes[_index]);
    }

}