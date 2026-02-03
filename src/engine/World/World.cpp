#include "engine/Precomp.h"
#include "engine/World/Scene/Scene.h"
#include "World.h"
#include "engine/Engine.h"
#include "engine/EngineOptions.h"
#include "engine/Component/Physics/Object/Body/PhysicsBodyComponent.h"
#include "engine/Component/Physics/Object/MergeStaticColliders/MergeStaticCollidersComponent.h"
#include "engine/Component/Physics/Shape/PhysicsShapeComponent.h"
#include "physics/IPhysics.h"
#include "physics/IPhysicsOptions.h"
#include "physics/IBody.h"
#include "physics/IShape.h"
#include "core/IInput.h"
#include "core/Time/Timer.h"
#include "core/Math/Math.h"
#include "renderer/IUIRenderer.h"
#include "gfx/ITexture.h"
#include "renderer/IRenderer.h"
#include "renderer/IRendererOptions.h"
#include "renderer/IDebugDraw.h"
#include "core/GameObject/GatherComponentsContext.h"
#include "ComponentUpdateJob.h"
#include "core/Scheduler/Scheduler.h"
#include "core/string/string.h"

#if !VG_ENABLE_INLINE
#include "World.inl"
#endif

#include "GameObjectHierarchy.hpp"
#include "Scene/Scene.hpp"
#include "Prefab/PrefabScene.hpp"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_CLASS(World, "World");

    //--------------------------------------------------------------------------------------
    World::World(const string & _name, IObject * _parent) :
        IWorld(_name, _parent)
    {
        for (uint j = 0; j < enumCount<BaseSceneType>(); ++j)
            m_activeScene[j] = nullptr;

        SetPhysicsWorld(Engine::get()->GetPhysics()->CreatePhysicsWorld(this));
        Engine::get()->registerWorld(this);
        
        SetDebugDrawData(Engine::get()->GetRenderer()->GetDebugDraw()->CreateDebugDrawData());

        core::Scheduler * jobScheduler = (core::Scheduler *)Kernel::getScheduler();
        const uint workerThreadCount = jobScheduler->GetWorkerThreadCount();
        m_componentUpdateJobPool.resize(workerThreadCount);
        for (uint i = 0; i < m_componentUpdateJobPool.size(); ++i)
            m_componentUpdateJobPool[i] = new ComponentUpdateJob(fmt::sprintf("ComponentUpdateJob %u", i));
    }

    //--------------------------------------------------------------------------------------
    World::~World()
    {
        Engine::get()->unregisterWorld(this);

        VG_SAFE_RELEASE(m_physicsWorld);
        VG_SAFE_RELEASE(m_environmentCubemap);
        VG_SAFE_RELEASE(m_irradianceCubemap);
        VG_SAFE_RELEASE(m_specularReflectionCubemap);
        VG_SAFE_DELETE(m_debugDrawData);

        for (uint i = 0; i < m_componentUpdateJobPool.size(); ++i)
            VG_SAFE_DELETE(m_componentUpdateJobPool[i]);
        m_componentUpdateJobPool.clear();

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
        return asBool(ObjectRuntimeFlags::Prefab & getObjectRuntimeFlags());
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
            _scene->AddRef();
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
            _scene->Release();
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
    // This is a hack to fix issue when a Prefab is being release (e.g., "Save & Update")
    // As it happens during render, we might end up rendering UI that is using objects that just been released
    // But it can cause flickering in all views
    //--------------------------------------------------------------------------------------
    void World::clearUI()
    {
        auto * renderer = Engine::get()->GetRenderer();
        for (auto target = 0; target < enumCount<gfx::ViewTarget>(); ++target)
        {
            for (auto * view : renderer->GetViews((gfx::ViewTarget)target))
                view->GetUIRenderer()->Clear();
        }

        for (auto target = 0; target < enumCount<gfx::ViewportTarget>(); ++target)
        {
            for (auto * viewport : renderer->GetViewports((gfx::ViewportTarget)target))
                viewport->GetUIRenderer()->Clear();
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

                    root->OnEnable(); // TODO: should not be needed

                    root->OnPlay();                    
                }
            }
        }

        // Warmup physics
        if (auto * physicsWorld = GetPhysicsWorld())
        {
            // Merge physics colliders
            if (Engine::get()->GetPhysics()->GetOptions()->IsMergeStaticBodiesEnabled())
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

        // Foreach scene, look if a MergeStaticObjectComponent is available
        const auto sceneCount = GetSceneCount(BaseSceneType::Scene);
        for (uint i = 0; i < sceneCount; ++i)
        {
            const IBaseScene * scene = GetScene(i, BaseSceneType::Scene);
            if (nullptr != scene)
            {
                IGameObject * root = VG_SAFE_STATIC_CAST(IGameObject, scene->GetRoot());
                if (nullptr != root)
                {
                    auto * mergeStaticCollidersComponent = root->GetComponentInChildrenT<MergeStaticCollidersComponent>();

                    if (nullptr == mergeStaticCollidersComponent)
                        mergeStaticCollidersComponent = root->AddComponentT<MergeStaticCollidersComponent>("MergeStaticColliders");

                    mergeStaticCollidersComponent->SetObjectRuntimeFlags(ObjectRuntimeFlags::Temporary, true);
        
                    IFactory * factory = Kernel::getFactory();
                    physics::IBodyDesc * bodyDesc = (physics::IBodyDesc *)factory->CreateObject("PhysicsBodyDesc", "", this);

                    vector<physics::ShapeInfo> allSceneShapes;
                    auto bodies = root->GetComponentsInChildrenT<PhysicsBodyComponent>();

                    float mass = 0.0f;

                    for (uint j = 0; j < bodies.size(); ++j)
                    {
                        PhysicsBodyComponent * bodyComponent = bodies[j];                        

                        if (!bodyComponent->isEnabled() || !bodyComponent->getBodyDesc()->CanStaticMerge())
                            continue;

                        auto componentShapes = bodyComponent->getShapes();
                        for (uint k = 0; k < componentShapes.size(); ++k)
                        {
                            const auto * bodyShape = componentShapes[k];
                            physics::ShapeInfo & info = allSceneShapes.emplace_back();
                            info.m_shape = bodyShape->getPhysicsShape();

                            float4x4 matrix = mul(bodyComponent->GetGameObject()->GetGlobalMatrix(), inverse(root->GetGlobalMatrix()));

                            float3x3 rot = extractRotation(matrix);
                            quaternion quat = quaternion(rot);

                            info.m_rotation = quat;
                            info.m_translation = matrix[3].xyz;

                            mass += info.m_shape->GetMass();
                        }

                        totalMergedCount += componentShapes.size();

                        bodyComponent->Enable(false);
                        VG_SAFE_INCREASE_REFCOUNT(bodyComponent);
                        m_staticBodyComponents.push_back(bodyComponent);
                    }

                    bodyDesc->SetMass(mass);

                    physics::IBody * body = Engine::get()->GetPhysics()->CreateBody(GetPhysicsWorld(), bodyDesc, allSceneShapes, root->GetGlobalMatrix(), "MergeStaticColliders", mergeStaticCollidersComponent);
                    body->Activate(root->GetGlobalMatrix());

                    m_staticBodies.push_back(body);

                    VG_SAFE_RELEASE(bodyDesc);
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
    bool destroyTemporaryObjectsRecur(IGameObject * _object)
    {
        auto & components = _object->GetComponents();
        if (components.size() > 0)
        {
            for (int i = (int)components.size() - 1; i >= 0; --i)
            {
                auto * component = components[i];
                if (asBool(ObjectRuntimeFlags::Temporary & component->GetObjectRuntimeFlags()))
                {
                    _object->RemoveComponent(component);
                    VG_SAFE_RELEASE(component);
                }
            }
        }

        if (asBool(ObjectRuntimeFlags::Temporary & _object->GetObjectRuntimeFlags()))
        {
            IGameObject * parent = VG_SAFE_STATIC_CAST(IGameObject, _object->getParent());
            parent->RemoveChild(_object);
            return true;
        }
        else
        {
            auto & children = _object->GetChildren();
            for (uint i = 0; i < children.size(); ++i)
            {
                if (destroyTemporaryObjectsRecur((IGameObject *)children[i]))
                    --i;
            }
        }
        return false;
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

                    #if 1
                    // Should not be needed but issues with physics components if no Disable/Enable on engine stop
                    root->OnDisable();
                    root->OnStop();
                    root->OnEnable();
                    #else
                    root->OnStop();
                    #endif
                }

                // Force refresh global matrices after restore or call OnPropertyChanged from RestoreProperties?
                ((GameObject*)root)->OnLocalMatrixChanged(false, true);
            }
        }

        for (uint i = 0; i < m_staticBodyComponents.size(); ++i)
            VG_SAFE_RELEASE(m_staticBodyComponents[i]);
        m_staticBodyComponents.clear();

        for (uint i = 0; i < m_staticBodies.size(); ++i)
            VG_SAFE_RELEASE(m_staticBodies[i]);
        m_staticBodies.clear();

        // Destroy temporary GameObjects
        for (uint i = 0; i < GetSceneCount(BaseSceneType::Scene); ++i)
        {
            const IBaseScene * scene = GetScene(i, BaseSceneType::Scene);
            if (nullptr != scene)
            {
                IGameObject * root = VG_SAFE_STATIC_CAST(IGameObject, scene->GetRoot());
                if (nullptr != root)
                    destroyTemporaryObjectsRecur(root);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void World::fixedUpdate(const Context & _context)
    {
        const GameObject::Context gameObjectUpdateContext(_context, this);

        for (uint i = 0; i < GetSceneCount(BaseSceneType::Scene); ++i)
        {
            const Scene * scene = (Scene *)GetScene(i, BaseSceneType::Scene);
            GameObject * root = scene->getRoot();
            if (root && asBool(UpdateFlags::FixedUpdate & root->getUpdateFlags()))
                root->fixedUpdate(gameObjectUpdateContext);
        }
    }

    //--------------------------------------------------------------------------------------
    void World::beforeUpdate(const Context & _context)
    {
        const auto * options = Engine::get()->GetRenderer()->GetOptions();

        // Use default cubemap/background clear color from options if not overridden by environment
        m_nextEnvironmentColor = options->GetDefaultClearColor();

        if (auto defaultCubemap = options->GetDefaultCubemap())
            m_defaultEnvironmentCubemap = defaultCubemap;
        else
            m_defaultEnvironmentCubemap = nullptr;

        // Reset cubemap
        VG_SAFE_RELEASE_ASYNC(m_environmentCubemap);

        // Use default ambient
        m_irradianceIntensity = options->GetDefaultIrradianceIntensity();
        m_specularReflectionIntensity = options->GetDefaultSpecularReflectionIntensity();
    }

    //--------------------------------------------------------------------------------------
    ComponentUpdateJob * World::getComponentUpdateJobFromPool()
    {
        VG_ASSERT(m_componentUpdateJobPoolIndex < m_componentUpdateJobPool.size());
        return m_componentUpdateJobPool[m_componentUpdateJobPoolIndex++];
    }

    //--------------------------------------------------------------------------------------
    void World::update(const Context & _context)
    {
        beforeUpdate(_context);

        const auto * options = EngineOptions::get();
        const bool useUpdateJobOrder = options->useJobsUpdateOrder();
        const bool useUpdateJobs = options->useComponentUpdateJobs();

        const GameObject::Context gameObjectUpdateContext(_context, this);

        if (useUpdateJobOrder)
        {
            m_gatherComponentsContext.reset();
            GatherComponentsContext & gatherComponentsContext = m_gatherComponentsContext;
            gatherComponentsContext.m_flags = UpdateFlags::Update;

            // Gather components to update
            {
                VG_PROFILE_CPU("getComponentsToUpdate");

                for (uint i = 0; i < GetSceneCount(BaseSceneType::Scene); ++i)
                {
                    const Scene * scene = (Scene *)GetScene(i, BaseSceneType::Scene);
                    GameObject * root = scene->getRoot();
                    if (root && asBool(UpdateFlags::Update & root->getUpdateFlags()))
                        root->getComponentsToUpdate(gatherComponentsContext);
                }
            }

            core::Scheduler * jobScheduler = (core::Scheduler *)Kernel::getScheduler();
            const uint workerThreadCount = jobScheduler->GetWorkerThreadCount();

            // execute in order
            ComponentUpdateContext componentUpdateContext(gameObjectUpdateContext, nullptr);
            for (uint g = 0; g < enumCount<ComponentGroup>(); ++g)
            {
                const ComponentGroup group = (ComponentGroup)g;
                VG_PROFILE_CPU(asCString(group));

                auto & priorityList = gatherComponentsContext.m_componentsToUpdate[g];

                for (uint p = 0; p < enumCount<ComponentPriority>(); ++p)
                {
                    const ComponentPriority prio = (ComponentPriority)p;
                    VG_PROFILE_CPU(asCString(prio));

                    auto & mtTypeList = priorityList[p];

                    for (uint m = 0; m < enumCount<ComponentMultithreadType>(); ++m)
                    {
                        const ComponentMultithreadType mtType = (ComponentMultithreadType)m;

                        auto & list = mtTypeList[m];

                        const uint componentCount = (uint)list.m_pairs.size();

                        if (useUpdateJobs && mtType == ComponentMultithreadType::Job && componentCount > 8)
                        {
                            // run N job and wait
                            VG_PROFILE_CPU("Jobs");
                            // Determine how many jobs to run
                            const uint maxJobs = (uint)m_componentUpdateJobPool.size();
                            const uint N = min(componentCount, maxJobs);
                            const uint minElementsPerJob = 1;

                            uint numJobs = N;

                            // If the calculated base size would be below the minimum, reduce the number of jobs
                            if (componentCount / N < minElementsPerJob)
                            {
                                numJobs = componentCount / minElementsPerJob;
                                numJobs = clamp(numJobs, uint(1), maxJobs); // ensure at least 1 job, no more than pool size
                            }

                            const uint baseSize = componentCount / N;
                            const uint remainder = componentCount % N;

                            uint index = 0;
                            uint remaining = componentCount;

                            core::JobSync renderJobSync;

                            // Init
                            //ComponentUpdateJob::s_counter = 0;
                            {
                                VG_PROFILE_CPU("Start");
                                for (uint n = 0; n < N; ++n)
                                {
                                    const uint count = baseSize + (n < remainder ? 1 : 0);
                                    VG_ASSERT(count > 0);
                                    auto components = core::span<GatherComponentsContext::Pair>(list.m_pairs.data() + index, count);
                                    VG_ASSERT(index + count <= list.m_pairs.size());
                                    ComponentUpdateJob * job = getComponentUpdateJobFromPool();
                                    job->init(group, prio, componentUpdateContext, components);
                                    index += count;
                                    remaining -= count;
                                    jobScheduler->Start(job, &renderJobSync);

                                    //VG_DEBUGPRINT("jobID = 0x%016llx\n", list.m_jobSync.id);
                                }
                            }

                            // Wait for job to complete
                            {
                                VG_PROFILE_CPU("Wait");
                                jobScheduler->Wait(&renderJobSync);

                                // Reset job pool
                                m_componentUpdateJobPoolIndex = 0;

                                //// Paranoid check that all jobs were completed
                                //uint value = ComponentUpdateJob::s_counter.load(std::memory_order_acquire);
                                //VG_ASSERT(value == N); 
                            }
                        }
                        else
                        {
                            // Not using jobs but using job update order on main thread
                            for (uint c = 0; c < componentCount; ++c)
                            {
                                auto & entry = list.m_pairs[c];
                                VG_PROFILE_CPU(entry.m_component->GetClassName());
                                componentUpdateContext.m_gameObject = entry.m_gameObject;
                                entry.m_component->Update(componentUpdateContext);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            for (uint i = 0; i < GetSceneCount(BaseSceneType::Scene); ++i)
            {
                const Scene * scene = (Scene *)GetScene(i, BaseSceneType::Scene);
                GameObject * root = scene->getRoot();
                if (root && asBool(UpdateFlags::Update & root->getUpdateFlags()))
                    root->update(gameObjectUpdateContext);
            }
        }

        afterUpdate(_context);
    }

    //--------------------------------------------------------------------------------------
    void World::afterUpdate(const Context & _context)
    {
        if (any(m_nextEnvironmentColor != m_currentEnvironmentColor))
        {
            // We must reset the render target pool when clear color actually changed
            Engine::get()->GetRenderer()->SetResized();
            m_currentEnvironmentColor = m_nextEnvironmentColor;
        }
    }

    //--------------------------------------------------------------------------------------
    void World::lateUpdate(const Context & _context)
    {
        const GameObject::Context gameObjectUpdateContext(_context, this);

        for (uint i = 0; i < GetSceneCount(BaseSceneType::Scene); ++i)
        {
            const Scene * scene = (Scene *)GetScene(i, BaseSceneType::Scene);
            GameObject * root = scene->getRoot();
            if (root && asBool(UpdateFlags::LateUpdate & root->getUpdateFlags()))
                root->lateUpdate(gameObjectUpdateContext);
        }
    }

    //--------------------------------------------------------------------------------------
    void World::toolUpdate(const Context & _context)
    {
        if (anyToolmodeViewVisible())
        {
            const GameObject::Context gameObjectUpdateContext(_context, this);

            for (uint i = 0; i < GetSceneCount(BaseSceneType::Scene); ++i)
            {
                const Scene * scene = (Scene *)GetScene(i, BaseSceneType::Scene);
                GameObject * root = scene->getRoot();
                if (root && asBool(UpdateFlags::ToolUpdate & root->getUpdateFlags()))
                    root->toolUpdate(gameObjectUpdateContext);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    bool World::anyToolmodeViewVisible() const
    {
        const auto & renderer = Engine::get()->GetRenderer();
        auto & editorViews = renderer->GetViews(gfx::ViewTarget::Editor);
        for (auto & view : editorViews)
        {
            if (view->IsRender())
            {
                if (view->IsToolmode())
                {
                    if (view->GetWorld() == this)
                        return true;
                }
            }
        }
        auto & gameViews = renderer->GetViews(gfx::ViewTarget::Game);
        for (auto & view : gameViews)
        {
            if (view->IsRender())
            {
                if (view->IsToolmode())
                {
                    if (view->GetWorld() == this)
                        return true;
                }
            }
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    void World::SetEnvironmentColor(const core::float4 & _environmentColor)
    {
        m_nextEnvironmentColor = _environmentColor;
    }

    //--------------------------------------------------------------------------------------
    core::float4 World::GetEnvironmentColor() const
    {
        return m_currentEnvironmentColor;
    }

    //--------------------------------------------------------------------------------------
    void World::SetEnvironmentCubemap(gfx::ITexture * _texture)
    {
        if (_texture != m_environmentCubemap)
        {
            VG_SAFE_RELEASE(m_environmentCubemap);
            m_environmentCubemap = _texture;
            VG_SAFE_INCREASE_REFCOUNT(m_environmentCubemap);
        }
    }

    //--------------------------------------------------------------------------------------
    gfx::ITexture * World::GetEnvironmentCubemap() const
    {
        return m_environmentCubemap ? m_environmentCubemap : m_defaultEnvironmentCubemap;
    }

    //--------------------------------------------------------------------------------------
    void World::SetIrradianceCubemap(gfx::ITexture * _texture)
    {
        if (m_irradianceCubemap != _texture)
        {
            VG_SAFE_RELEASE(m_irradianceCubemap);
            m_irradianceCubemap = _texture;
            VG_SAFE_INCREASE_REFCOUNT(m_irradianceCubemap);
        }
    }

    //--------------------------------------------------------------------------------------
    gfx::ITexture * World::GetIrradianceCubemap() const
    {
        return m_irradianceCubemap;
    }

    //--------------------------------------------------------------------------------------
    void World::SetSpecularReflectionCubemap(gfx::ITexture * _texture)
    {
        if (m_specularReflectionCubemap != _texture)
        {
            VG_SAFE_RELEASE_ASYNC(m_specularReflectionCubemap);
            m_specularReflectionCubemap = _texture;
            VG_SAFE_INCREASE_REFCOUNT(m_specularReflectionCubemap);
        }
    }

    //--------------------------------------------------------------------------------------
    gfx::ITexture * World::GetSpecularReflectionCubemap() const
    {
        return m_specularReflectionCubemap;
    }

    //--------------------------------------------------------------------------------------
    void World::SetIrradianceIntensity(float _value)
    {
        m_irradianceIntensity = _value;
    }

    //--------------------------------------------------------------------------------------
    float World::GetIrradianceIntensity() const
    {
        return m_irradianceIntensity;
    }

    //--------------------------------------------------------------------------------------
    void World::SetSpecularReflectionIntensity(float _value) 
    {
        m_specularReflectionIntensity = _value;
    }

    //--------------------------------------------------------------------------------------
    float World::GetSpecularReflectionIntensity() const 
    {
        return m_specularReflectionIntensity;
    }
}