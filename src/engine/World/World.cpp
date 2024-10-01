#include "engine/Precomp.h"
#include "engine/World/Scene/Scene.h"
#include "World.h"
#include "engine/Engine.h"
#include "engine/EngineOptions.h"
#include "engine/Component/Physics/Body/PhysicsBodyComponent.h"
#include "engine/Component/Physics/Shape/PhysicsShapeComponent.h"
#include "physics/IPhysics.h"
#include "physics/IBody.h"
#include "physics/IShape.h"
#include "core/IInput.h"
#include "core/Timer/Timer.h"
#include "core/Math/Math.h"
#include "gfx/IViewGUI.h"
#include "renderer/IRenderer.h"
#include "renderer/IDebugDraw.h"

#if !VG_ENABLE_INLINE
#include "World.inl"
#endif

#include "GameObjectHierarchy.hpp"
#include "Scene/Scene.hpp"
#include "Prefab/PrefabScene.hpp"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(World, "World");

    //--------------------------------------------------------------------------------------
    World::World(const string & _name, IObject * _parent) :
        IWorld(_name, _parent)
    {
        for (uint j = 0; j < enumCount<BaseSceneType>(); ++j)
            m_activeScene[j] = nullptr;

        SetPhysicsWorld(Engine::get()->GetPhysics()->CreatePhysicsWorld(this));
        Engine::get()->registerWorld(this);
        ;
        SetDebugDrawData(Engine::get()->GetRenderer()->GetDebugDraw()->CreateDebugDrawData());
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
    vector<core::IBaseScene*> & World::getScenes(BaseSceneType _sceneType)
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
    bool World::IsPrefabWorld() const
    {
        return asBool(ObjectFlags::Prefab & getObjectFlags());
    }

    //--------------------------------------------------------------------------------------
    bool World::SetActiveScene(IBaseScene * _scene, BaseSceneType _sceneType)
    {
        const auto typeIndex = asInteger(_sceneType);

        if (nullptr == _scene)
        {
            if (nullptr != m_activeScene[typeIndex])
            {
                m_activeScene[typeIndex] = nullptr;
                return true;
            }
        }
        else
        {
            if (_scene != m_activeScene[typeIndex])
            {
                if (vector_helper::exists(m_scenes[typeIndex], (IBaseScene *)_scene))
                {
                    m_activeScene[typeIndex] = (Scene *)_scene;
                    return true;
                }
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
        if (nullptr != _scene && !vector_helper::exists(m_scenes[typeIndex], (IBaseScene *)_scene))
        {
            _scene->SetParent(this);
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
        if (nullptr != _scene && vector_helper::exists(m_scenes[typeIndex], _scene))
        {
            vector_helper::remove(m_scenes[typeIndex], _scene);

            if (m_activeScene[typeIndex] == _scene)
                SetActiveScene(nullptr, _sceneType);

            _scene->SetParent(nullptr);
            _scene->release();
        }

        clearUI();

        return false;
    }

    //--------------------------------------------------------------------------------------
    uint World::RemoveAllScenes(BaseSceneType _sceneType)
    {
        const auto typeIndex = asInteger(_sceneType);
        const auto sceneCount = m_scenes[typeIndex].size();
        for (uint i = 0; i < sceneCount; ++i)
        {
            auto & scene = m_scenes[typeIndex][i];
            scene->SetParent(nullptr);
            VG_SAFE_RELEASE(scene);
        }
        m_scenes[typeIndex].clear();
        SetActiveScene(nullptr, _sceneType);

        clearUI();

        return (uint)sceneCount;
    }

    //--------------------------------------------------------------------------------------
    // This is a hack to fix issue when a Prefab is being release (e.g. "Save & Update")
    // As it happens during render, we might end up rendering UI that is using objects that just been released
    // But it can cause flickering in all views
    //--------------------------------------------------------------------------------------
    void World::clearUI()
    {
        auto * renderer = Engine::get()->GetRenderer();
        for (auto target = 0; target < enumCount<gfx::ViewTarget>(); ++target)
        {
            for (auto * view : renderer->GetViews((gfx::ViewTarget)target))
                view->GetViewGUI()->Clear();
        }

        for (auto target = 0; target < enumCount<gfx::ViewportTarget>(); ++target)
        {
            for (auto * viewport : renderer->GetViewports((gfx::ViewportTarget)target))
                viewport->GetViewportGUI()->Clear();
        }
    }

    //--------------------------------------------------------------------------------------
    uint World::GetSceneCount(BaseSceneType _sceneType) const
    {
        const auto typeIndex = asInteger(_sceneType);
        return (uint)m_scenes[typeIndex].size();
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
        return /*!IsPrefabWorld() &&*/ Engine::get()->IsPlaying(); // TODO: world should be part of the context update
    }

    //--------------------------------------------------------------------------------------
    bool World::IsPaused() const
    {
        return Engine::get()->IsPaused();
    }

    //--------------------------------------------------------------------------------------
    void World::OnPlay()
    {
        IFactory * factory = Kernel::getFactory();

        for (uint i = 0; i < GetSceneCount(BaseSceneType::Scene); ++i)
        {
            const IBaseScene * scene = GetScene(i, BaseSceneType::Scene);
            if (nullptr != scene)
            {
                IObject * root = scene->GetRoot();
                if (nullptr != root)
                {
                    factory->SaveProperties(root, BufferType::InitValue);

                    root->OnEnable();
                    root->OnPlay();                    
                }
            }
        }

        // Warmup physics
        if (auto * physicsWorld = GetPhysicsWorld())
        {
            // Merge physics colliders
            if (EngineOptions::get()->mergeStaticBodies())
                mergeStaticBodies();

            physicsWorld->OnPlay();
        }
    }

    //--------------------------------------------------------------------------------------
    void World::mergeStaticBodies()
    {
        const auto startMergeStaticBodies = Timer::getTick();

        // For each scene, gather static colliders
        size_t totalMergedCount = 0;

        for (uint i = 0; i < GetSceneCount(BaseSceneType::Scene); ++i)
        {
            const IBaseScene * scene = GetScene(i, BaseSceneType::Scene);
            if (nullptr != scene)
            {
                IGameObject * root = VG_SAFE_STATIC_CAST(IGameObject, scene->GetRoot());
                if (nullptr != root)
                {
                    auto bodies = root->GetComponentsInChildrenT<PhysicsBodyComponent>();

                    // Add "_Temp" folder
                    auto * tempGO = root->GetChildGameObject("_Temp");
                    if (nullptr == tempGO)
                    {
                        tempGO = new GameObject("_Temp", root);
                        root->AddChild(tempGO);
                        tempGO->release();
                    }

                    // Add "_Static" GameObject
                    GameObject * staticCollidersGO = new GameObject("_Static", tempGO);
                    tempGO->AddChild(staticCollidersGO);
                    staticCollidersGO->release();

                    IFactory * factory = Kernel::getFactory();
                    physics::IBodyDesc * bodyDesc = (physics::IBodyDesc *)factory->CreateObject("PhysicsBodyDesc", "", this);
                    
                    vector<physics::ShapeInfo> allSceneShapes;
                    
                    float mass = 0.0f;

                    for (uint j = 0; j < bodies.size(); ++j)
                    {
                        PhysicsBodyComponent * bodyComponent = bodies[j];

                        if (bodyComponent->getBodyDesc()->GetMotion() != physics::MotionType::Static)
                            continue;

                        auto componentShapes = bodyComponent->getShapes();
                        for (uint k = 0; k < componentShapes.size(); ++k)
                        {
                            const auto * bodyShape = componentShapes[k];
                            physics::ShapeInfo & info = allSceneShapes.emplace_back();
                            info.m_shape = bodyShape->getPhysicsShape();

                            float4x4 matrix = bodyComponent->GetGameObject()->GetGlobalMatrix();

                            float3x3 rot = extractRotation(matrix);
                            quaternion quat = quaternion(rot);

                            info.m_rotation = quat;
                            info.m_translation = matrix[3].xyz;

                            mass += info.m_shape->GetMass();
                        }

                        totalMergedCount += componentShapes.size();
                    }

                    bodyDesc->SetMass(mass);
                    
                    physics::IBody * body = Engine::get()->GetPhysics()->CreateBody(GetPhysicsWorld(), bodyDesc, allSceneShapes, staticCollidersGO->GetGlobalMatrix(), staticCollidersGO->GetName(), staticCollidersGO);
                    body->Activate(staticCollidersGO->GetGlobalMatrix());

                    m_staticColliders.push_back(body);
                    
                    VG_SAFE_RELEASE(bodyDesc);

                    //for (uint j = 0; j < allSceneShapes.size(); ++j)
                    //    VG_SAFE_RELEASE(allSceneShapes[j]);
                    allSceneShapes.clear();
                }
            }
        }
        
        VG_INFO("[Physics] Merge %u static bodies in %.2f ms", totalMergedCount, Timer::getEnlapsedTime(startMergeStaticBodies, Timer::getTick()));
    }

    //--------------------------------------------------------------------------------------
    void World::OnPause()
    {

    }

    //--------------------------------------------------------------------------------------
    void World::OnResume()
    {

    }

    //--------------------------------------------------------------------------------------
    void World::OnStop()
    {
        IFactory * factory = Kernel::getFactory();

        for (uint i = 0; i < GetSceneCount(BaseSceneType::Scene); ++i)
        {
            const IBaseScene * scene = GetScene(i, BaseSceneType::Scene);
            if (nullptr != scene)
            {
                IObject * root = scene->GetRoot();
                if (nullptr != root)
                {
                    factory->RestoreProperties(root, BufferType::InitValue);

                    root->OnDisable();
                    root->OnStop();
                }
            }
        }

        for (uint i = 0; i < m_staticColliders.size(); ++i)
            VG_SAFE_RELEASE(m_staticColliders[i]);
        m_staticColliders.clear();

        // Destroy "_Temp"
        for (uint i = 0; i < GetSceneCount(BaseSceneType::Scene); ++i)
        {
            const IBaseScene * scene = GetScene(i, BaseSceneType::Scene);
            if (nullptr != scene)
            {
                IGameObject * root = VG_SAFE_STATIC_CAST(IGameObject, scene->GetRoot());
                if (nullptr != root)
                {
                    if (auto * tempGO = root->GetChildGameObject("_Temp"))
                        root->RemoveChild(tempGO);
                }
            }
        }
    }
}