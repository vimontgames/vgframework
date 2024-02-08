#include "engine/Precomp.h"
#include "engine/World/Scene/Scene.h"
#include "World.h"
#include "engine/Engine.h"
#include "physics/IPhysics.h"
#include "core/IInput.h"

#if !VG_ENABLE_INLINE
#include "World.inl"
#endif

#include "GameObjectHierarchy.hpp"
#include "Scene/Scene.hpp"
#include "Prefab/Prefab.hpp"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(World, "World");

    //--------------------------------------------------------------------------------------
    World::World(const string & _name, IObject * _parent) :
        IWorld(_name, _parent),
        m_isPlaying(false),
        m_isPaused(false)
    {
        for (uint j = 0; j < enumCount<BaseSceneType>(); ++j)
            m_activeScene[j] = nullptr;

        SetPhysicsWorld(Engine::get()->GetPhysics()->CreatePhysicsWorld(this));
        Engine::get()->registerWorld(this);
    }

    //--------------------------------------------------------------------------------------
    World::~World()
    {
        Engine::get()->unregisterWorld(this);

        VG_SAFE_RELEASE(m_physicsWorld);

        VG_SAFE_DELETE(m_debugDrawData);

        for (uint j = 0; j < enumCount<BaseSceneType>(); ++j)
        {
            auto sceneType = (BaseSceneType)j;
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
    vector<Scene*> & World::getScenes(BaseSceneType _sceneType)
    {
        return m_scenes[asInteger(_sceneType)];
    }

    //--------------------------------------------------------------------------------------
    void World::SetDebugDrawData(core::IDebugDrawData * _debugDrawData)
    {
        VG_ASSERT(!m_debugDrawData);
        m_debugDrawData = _debugDrawData;
    }
    
    //--------------------------------------------------------------------------------------
    core::IDebugDrawData * World::GetDebugDrawData() const
    {
        return m_debugDrawData;
    }

    //--------------------------------------------------------------------------------------
    void World::SetPhysicsWorld(core::IPhysicsWorld * _physicsWorld)
    {
        VG_ASSERT(!m_physicsWorld);
        m_physicsWorld = _physicsWorld;
    }

    //--------------------------------------------------------------------------------------
    core::IPhysicsWorld * World::GetPhysicsWorld() const
    {
        return m_physicsWorld;
    }

    //--------------------------------------------------------------------------------------
    bool World::SetActiveScene(IBaseScene * _scene, BaseSceneType _sceneType)
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
    core::IBaseScene * World::GetActiveScene(BaseSceneType _sceneType) const
    {
        const auto typeIndex = asInteger(_sceneType);
        return m_activeScene[typeIndex];
    }

    //--------------------------------------------------------------------------------------
    bool World::AddScene(IBaseScene * _scene, BaseSceneType _sceneType)
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
    bool World::RemoveScene(IBaseScene * _scene, BaseSceneType _sceneType)
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
    uint World::RemoveAllScenes(BaseSceneType _sceneType)
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
    uint World::GetSceneCount(BaseSceneType _sceneType) const
    {
        const auto typeIndex = asInteger(_sceneType);
        return m_scenes[typeIndex].count();
    }

    //--------------------------------------------------------------------------------------
    const IBaseScene * World::GetScene(uint _index, BaseSceneType _sceneType) const
    {
        const auto typeIndex = asInteger(_sceneType);
        return reinterpret_cast<const IBaseScene*>(m_scenes[typeIndex][_index]);
    }

    //--------------------------------------------------------------------------------------
    bool World::IsPlaying() const
    {
        return isPlaying();
    }

    //--------------------------------------------------------------------------------------
    bool World::IsPaused() const
    {
        return isPaused();
    }

    //--------------------------------------------------------------------------------------
    void World::Play()
    {
        play();
    }

    //--------------------------------------------------------------------------------------
    void World::Stop()
    {
        stop();
    }

    //--------------------------------------------------------------------------------------
    void World::Pause()
    {
        pause();
    }

    //--------------------------------------------------------------------------------------
    void World::Resume()
    {
        resume();
    }

    //--------------------------------------------------------------------------------------
    void World::play()
    {
        VG_INFO("[World] Play world \"%s\"", getName().c_str());
        m_isPlaying = true;
        m_isPaused = false;

        // Detect joypads
        Kernel::getInput()->OnPlay();

        for (uint i = 0; i < GetSceneCount(BaseSceneType::Scene); ++i)
        {
            const IBaseScene * scene = GetScene(i, BaseSceneType::Scene);
            if (nullptr != scene)
            {
                IObject * root = scene->GetRoot();
                if (nullptr != root)
                    root->OnPlay();
            }
        }

        // Warmup physics
        if (auto * physicsWorld = GetPhysicsWorld())
            physicsWorld->OnPlay();
    }

    //--------------------------------------------------------------------------------------
    void World::pause()
    {
        VG_INFO("[World] Pause world \"%s\"", getName().c_str());
        VG_ASSERT(m_isPlaying);
        m_isPaused = true;
    }

    //--------------------------------------------------------------------------------------
    void World::resume()
    {
        VG_INFO("[World] Resume world \"%s\"", getName().c_str());
        VG_ASSERT(m_isPlaying && m_isPaused);
        m_isPaused = false;
    }

    //--------------------------------------------------------------------------------------
    void World::stop()
    {
        VG_INFO("[World] Stop world \"%s\"", getName().c_str());
        m_isPlaying = false;
        m_isPaused = false;

        for (uint i = 0; i < GetSceneCount(BaseSceneType::Scene); ++i)
        {
            const IBaseScene * scene = GetScene(i, BaseSceneType::Scene);
            if (nullptr != scene)
            {
                IObject * root = scene->GetRoot();
                if (nullptr != root)
                    root->OnStop();
            }
        }
    }
}