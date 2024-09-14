#include "engine/Precomp.h"

#include "engine.h"
#include "engine/EngineOptions.h"
#include "core/Kernel.h"
#include "core/IProfiler.h"
#include "core/Timer/Timer.h"
#include "core/Plugin/Plugin.h"
#include "core/Logger/Logger.h"
#include "core/File/File.h"
#include "core/Scheduler/Scheduler.h"
#include "core/Object/Factory.h"
#include "core/GameObject/GameObject.h"
#include "core/Math/Math.h"
#include "gfx/IView.h"
#include "gfx/IDevice.h"
#include "renderer/IRenderer.h"
#include "renderer/IRendererOptions.h"
#include "physics/IPhysics.h"
#include "audio/IAudio.h"
#include "engine/World/World.h"
#include "engine/World/Scene/Scene.h"
#include "engine/Input/Input.h"
#include "engine/Resource/ResourceManager.h"
#include "engine/Resource/World/WorldResource.h"
#include "engine/Resource/World/WorldResourceData.h"
#include "engine/Selection/Selection.h"
#include "engine/Component/Camera/CameraComponent.h"
#include "engine/Component/Mesh/MeshComponent.h"
#include "engine/Behaviour/FreeCam/FreeCamBehaviour.h"
#include "editor/IEditor.h"
#include "application/IGame.h"

#if !VG_ENABLE_INLINE
#include "Engine.inl"
#endif

using namespace vg::core;
using namespace vg::engine;

#define VG_ENGINE_VERSION_MAJOR 0
#define VG_ENGINE_VERSION_MINOR 1

//--------------------------------------------------------------------------------------
IEngine * CreateNew()
{
	return new Engine("Engine", nullptr);
}

//--------------------------------------------------------------------------------------
namespace vg::engine
{
	//--------------------------------------------------------------------------------------
	IPlugin::Version Engine::GetVersion() const
	{
		return { VG_ENGINE_VERSION_MAJOR, VG_ENGINE_VERSION_MINOR };
	}

    #ifdef _WIN32
    //--------------------------------------------------------------------------------------
    LRESULT CALLBACK Engine::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (m_renderer && m_renderer->WndProc(hWnd, message, wParam, lParam))
            return true;

        switch (message)
        {
            case WM_GETMINMAXINFO: 
            {
                MINMAXINFO* pMinMaxInfo = reinterpret_cast<MINMAXINFO*>(lParam);
                pMinMaxInfo->ptMinTrackSize.x = 320; 
                pMinMaxInfo->ptMinTrackSize.y = 200; 
                return 0;
            }

            case WM_SIZE:
                if (m_renderer)
                {
                    const uint width = uint(lParam & 0xFFFF);
                    const uint height = uint(lParam >> 16);
                    m_renderer->Resize(width, height);
                }
                break;

            case WM_MOVE:
                if (m_renderer)
                    m_renderer->Move();
                break;

            case WM_PAINT:
                RunOneFrame();
                break;

            case WM_KEYDOWN:
                switch (wParam)
                {
                    case VK_F1:
                    {
                        VG_PROFILE_TRIGGER();   // Start/Stop capture
                    }
                    break;

                    case VK_ESCAPE:
                    {
                        if (IEngine * engine = Engine::get())
                        {
                            if (engine->IsPlaying())
                                engine->Stop();

                            if (m_renderer && m_renderer->IsFullscreen())
                                m_renderer->SetFullscreen(false);
                        }
                    }
                    break;;

                    case VK_F5:
                    {
                        if (IEngine * engine = Engine::get())
                        {
                            if (GetKeyState(VK_SHIFT) & 0x8000)
                            {
                                if (engine->IsPlaying())
                                {
                                    engine->Stop();
                                    engine->Play();
                                }
                            }
                            else
                            {
                                if (engine->IsPlaying())
                                    engine->Stop();
                                else
                                    engine->Play();
                            }
                        }
                    }
                    break;

                    case VK_F6:
                    {
                        if (m_renderer)
                            m_renderer->UpdateShaders();
                    }
                    break;

                    case VK_F7:
                    {
                        if (m_resourceManager)
                            m_resourceManager->UpdateResources();
                    }
                    break;

                    case VK_PAUSE:
                    {
                        if (IEngine * engine = Engine::get())
                        {
                            if (engine->IsPaused())
                                engine->Resume();
                            else
                                engine->Pause();
                        }
                    }
                    break;

                    case VK_F11:
                        toggleFullscreen();
                        break;
                }
                break;
        }

        return 0;
    }
    #endif

	//--------------------------------------------------------------------------------------
	Engine::Engine(const core::string & _name, core::IObject * _parent) :
        IEngine(_name, _parent),
        m_isPlaying(false),
        m_isPaused(false),
        m_startInPlayMode(false),
        m_quit(false)
	{
        
	}

	//--------------------------------------------------------------------------------------
	Engine::~Engine()
	{

	}

    //--------------------------------------------------------------------------------------
    bool Engine::RegisterClasses()
    {
        core::IFactory * factory = Kernel::getFactory();

        // Register engine *BEFORE* registering classes so as to be able to display asserts related to properties
        if (core::IClassDesc * desc = factory->registerPlugin(Engine, "Engine"))
            registerProperties(*desc);

        // Register classes to auto-register the "Engine" module
        AutoRegisterClassInfo::registerClasses(*factory);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Engine::UnregisterClasses()
    {
        return AutoRegisterClassInfo::unregisterClasses();
    }

    //--------------------------------------------------------------------------------------
    bool Engine::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    void Engine::OnPropertyChanged(IObject * _object, const IProperty & _prop, bool _notifyParent)
    {

    }

    //--------------------------------------------------------------------------------------
    bool Engine::createProject(core::IObject * _engine)
    {
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Engine::LoadGame(const string & _path)
    {
        if (_path.length() > 0)
        {
            auto folderNameBegin = _path.find_last_of('/');
            string gameDLLPath = _path + _path.substr(folderNameBegin);

            m_game = Plugin::create<IGame>(gameDLLPath);

            if (nullptr != m_game)
                m_game->Init(*this, Kernel::getSingletons());
        }
        return nullptr != m_game;
    }

    //--------------------------------------------------------------------------------------
    bool Engine::UnloadGame()
    {
        if (m_game)
        {
            m_game->Deinit();
            VG_SAFE_RELEASE(m_game);
            m_game = nullptr;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void Engine::registerWorld(World * _world)
    {
        VG_ASSERT(!vector_helper::exists(m_worlds, (IWorld*)_world));
        m_worlds.push_back(_world);
    }

    //--------------------------------------------------------------------------------------
    void Engine::unregisterWorld(World * _world)
    {
        VG_ASSERT(vector_helper::exists(m_worlds, (IWorld *)_world));
        vector_helper::remove(m_worlds, (IWorld *)_world);
    }

    //--------------------------------------------------------------------------------------
    IGame * Engine::GetGame() const
    {
        return m_game;
    }

    //--------------------------------------------------------------------------------------
    core::IWorld * Engine::GetMainWorld() const
    {
        if (m_worldResource)
        {
            core::IObject * obj = m_worldResource->GetObject();
            VG_ASSERT(obj == nullptr || dynamic_cast<WorldResourceData *>(obj));
            if (obj)
                return ((WorldResourceData *)obj)->getWorld();
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    const core::vector<core::IWorld *> & Engine::GetWorlds() const
    {
        return m_worlds;
    }

	//--------------------------------------------------------------------------------------
	void Engine::Init(const EngineCreationParams & _params, Singletons & _singletons)
	{
        Timer::init();

		string api;
		switch (_params.renderer.device.api)
		{
			default:
				VG_ASSERT(false);
				break;

			case gfx::API::DirectX12:
				api = "DX12";
				break;

			case gfx::API::Vulkan:
				api = "Vulkan";
				break;
		}

		const auto & name = asString(_params.renderer.device.api);

        // Singletons created by the engine
        if (nullptr != _params.logger)
            _singletons.logger = _params.logger;
        else
            _singletons.logger = new Logger();
        Kernel::setLogger(_singletons.logger);

        _singletons.scheduler = new Scheduler();
        Kernel::setScheduler(_singletons.scheduler);

        _singletons.input = new Input(_params.renderer.device.window);
        Kernel::setInput(_singletons.input);

        _singletons.factory = new Factory();
        Kernel::setFactory(_singletons.factory);

        m_selection = new Selection();
        _singletons.selection = m_selection;
        Kernel::setSelection(_singletons.selection);

        // Register Engine classes earlier so as to be able to display early error messages
        RegisterClasses();

        // Load engine options
        EngineOptions * engineOptions = new EngineOptions("EngineOptions", this);

        // Load Renderer DLL
		m_renderer = Plugin::create<renderer::IRenderer>("renderer", api);
		m_renderer->init(_params.renderer, _singletons);

        // Profiler has to be created by the renderer to be able to also profile the GPU
        Kernel::setProfiler(_singletons.profiler);
        _singletons.scheduler->RegisterCurrentThread("Main");

        // Register worker threads, it will be useful to get worker thread names in profiler
        _singletons.scheduler->RegisterWorkerThreads();

        // Load Physics DLL
        m_physics = Plugin::create<physics::IPhysics>("physics");
        m_physics->Init(_params.physics, _singletons);

        // Load Audio DLL
        m_audio = Plugin::create<audio::IAudio>("audio");
        m_audio->Init(_params.audio, _singletons);

        m_resourceManager = new ResourceManager("Resource Manager", this);

        // Load project DLL
        LoadGame(engineOptions->GetProjectPath());

        // Load Editor DLL
        #if VG_ENABLE_EDITOR
        m_editor = Plugin::create<editor::IEditor>("editor");
        m_editor->Init(_singletons);
        #endif

        // Create default world resource or load world path from commandline (TODO)
        VG_ASSERT(m_worldResource == nullptr);
        m_worldResource = new WorldResource("Default", this);
	}

    //--------------------------------------------------------------------------------------
    renderer::IRendererOptions * Engine::getRendererOptions() const
    {
        return m_renderer->GetOptions();
    }

    //--------------------------------------------------------------------------------------
    physics::IPhysicsOptions * Engine::getPhysicsOptions() const
    {
        return m_physics->GetOptions();
    }

    //--------------------------------------------------------------------------------------
    audio::IAudioOptions * Engine::getAudioOptions() const
    {
        return m_audio->GetOptions();
    }

    //--------------------------------------------------------------------------------------
    void Engine::onResourceLoaded(core::IResource * _resource)
    {
        if (_resource == m_worldResource)
        {
            VG_ASSERT(dynamic_cast<WorldResource*>(_resource));
            WorldResource * worldRes = (WorldResource *)_resource;
            auto world = worldRes->GetWorld();
            world->setName(io::getFileNameWithoutExt(worldRes->GetResourcePath()));

            auto & editorViews = m_renderer->GetViews(gfx::ViewTarget::Editor);
            for (auto view : editorViews)
            {
                if (view)
                {
                    if (!asBool(gfx::IView::Flags::Prefab & view->GetFlags()))
                        view->SetWorld(world);
                }
            }

            auto & gameViews = m_renderer->GetViews(gfx::ViewTarget::Game);
            for (auto view : gameViews)
            {
                if (view)
                {
                    if (!asBool(gfx::IView::Flags::Prefab & view->GetFlags()))
                        view->SetWorld(world);
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void Engine::onResourceUnloaded(core::IResource * _resource)
    {
        if (_resource == m_worldResource)
        {
            auto & editorViews = m_renderer->GetViews(gfx::ViewTarget::Editor);
            for (auto view : editorViews)
            {
                if (view)
                    view->SetWorld(nullptr);
            }

            auto & gameViews = m_renderer->GetViews(gfx::ViewTarget::Game);
            for (auto view : gameViews)
            {
                if (view)
                    view->SetWorld(nullptr);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    bool Engine::CreateWorld(const core::string & _filename)
    {
        m_worldResource->ClearResourcePath();

        if (m_worldResource->CreateFile(_filename))
        {
            m_worldResource->SetResourcePath(_filename);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Engine::SaveWorld()
    {
        IObject * world = m_worldResource->GetObject();
        if (world)
        {
            const auto filename = m_worldResource->GetResourcePath();
            world->setName(core::io::getFileNameWithoutExt(filename));
            return m_worldResource->SaveFile(filename);
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Engine::SaveWorldAs(const core::string & _filename)
    {
        IObject * world = m_worldResource->GetObject();
        if (world)
        {
            world->setName(core::io::getFileNameWithoutExt(_filename));
            return m_worldResource->SaveFile(_filename);
        }
        
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Engine::LoadWorld(const core::string & _filename)
    {
        if (m_worldResource->GetResourcePath() != _filename)
        {
            m_worldResource->SetResourcePath(_filename);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    IWorldResource * Engine::GetWorldResource()
    {
        return m_worldResource;
    }

    //--------------------------------------------------------------------------------------
    bool Engine::SaveScene(core::IResource * _sceneRes)
    {
        IBaseScene * scene = VG_SAFE_STATIC_CAST(IBaseScene, _sceneRes->GetObject());
        if (nullptr != scene)
        {
            if (scene->hasFile())
            {
                IFactory * factory = Kernel::getFactory();
                factory->saveToXML(scene, scene->getFile());
                return true;
            }
        }
        return false;        
    }

    //--------------------------------------------------------------------------------------
    IEngineOptions * Engine::GetOptions()
    {
        return EngineOptions::get();
    }

	//--------------------------------------------------------------------------------------
	void Engine::Deinit()
	{
        m_renderer->WaitGPUIdle();
        m_resourceManager->flushPendingLoading();

        UnloadGame();

        EngineOptions * engineOptions = EngineOptions::get();
        VG_SAFE_DELETE(engineOptions);

        UnregisterClasses();

        Kernel::setProfiler(nullptr);

        IScheduler * scheduler = Kernel::getScheduler();
        VG_SAFE_DELETE(scheduler);
        Kernel::setScheduler(nullptr);

        IInput * input = Kernel::getInput();
        VG_SAFE_DELETE(input);
        Kernel::setInput(nullptr);

        IFactory * factory = Kernel::getFactory();

        #if VG_ENABLE_EDITOR
        m_editor->Deinit();
        m_editor->Release();
        #endif

        // ~dtor time is too late to unload world resource we have to use ptr so as to do it manually before the ResourceMananger shutdowns
        VG_SAFE_RELEASE(m_worldResource);

        // Resource Manager should be deleted before renderer because the shared resource must be released to avoid GPU memory leak checked in gfx::Device deinit
        VG_SAFE_DELETE(m_resourceManager);

        m_physics->Deinit();
        VG_SAFE_RELEASE(m_physics);

        m_audio->Deinit();
        VG_SAFE_RELEASE(m_audio);

		m_renderer->deinit();
        VG_SAFE_RELEASE(m_renderer);

        VG_SAFE_DELETE(factory);
        Kernel::setFactory(nullptr);

        ILogger * logger = Kernel::getLogger();
        VG_SAFE_DELETE(logger);
        Kernel::setLogger(nullptr);

        VG_SAFE_RELEASE(m_selection);
        Kernel::setSelection(nullptr);
	}

    //--------------------------------------------------------------------------------------
    void Engine::StartInPlayMode(bool _enable)
    {
        m_startInPlayMode = _enable;
    }

    //--------------------------------------------------------------------------------------
    void Engine::Quit()
    {
        if (IsPlaying())
            Stop();

        m_quit = true;
    }

    //--------------------------------------------------------------------------------------
    bool Engine::IsQuitting() const
    {
        return m_quit;
    }

    //--------------------------------------------------------------------------------------
    void Engine::updateDt()
    {
        static Ticks previous = 0;
        Ticks current = Timer::getTick();
        const float dt = previous ? (float)(Timer::getEnlapsedTime(previous, current) * 0.001f) : 0.0f;
        m_time.m_realDeltaTime = dt;

        m_time.m_fps = 1.0f / m_time.m_realDeltaTime;

        auto * engineOptions = GetOptions();

        if (auto * mainWorld = GetMainWorld())
        {
            if (mainWorld->IsPlaying())
            {
                float fixedDT;
                if (engineOptions->TryGetFixedDT(fixedDT))
                {
                    m_time.m_scaledDeltaTime = fixedDT / 1000.0f;
                }
                else
                {
                    if (mainWorld->IsPaused())
                    {
                        m_time.m_scaledDeltaTime = 0.0f;
                    }
                    else
                    {
                        float timescale = 1.0f;
                        if (engineOptions->TryGetTimeScale(timescale))
                            m_time.m_scaledDeltaTime = m_time.m_realDeltaTime * timescale;
                        else
                            m_time.m_scaledDeltaTime = m_time.m_realDeltaTime;
                    }
                }
            }
            else
                m_time.m_scaledDeltaTime = 0.0f;

            float maxDT;
            if (engineOptions->TryGetMaxDT(maxDT))
                m_time.m_scaledDeltaTime = min(m_time.m_scaledDeltaTime, maxDT / 1000.0f);

            previous = current;
        }

        m_time.m_enlapsedTimeSinceStartReal += m_time.m_realDeltaTime;
        m_time.m_enlapsedTimeSinceStartScaled += m_time.m_scaledDeltaTime;
        m_time.m_gpu = (float)m_renderer->GetGpuFrameTime();
        m_time.m_gpuWait = (float)m_renderer->GetGpuWaitTime();

        // Smooth values
        m_timeInternal.m_counter++;
        m_timeInternal.m_dtSum += m_time.m_realDeltaTime;
        m_timeInternal.m_gpuSum += m_time.m_gpu;
        m_timeInternal.m_gpuWaitSum += m_time.m_gpuWait;
        
        if (m_timeInternal.m_dtSum > 1.0f)
        {
            m_time.smoothed.m_dt = (float)(m_timeInternal.m_dtSum / (float)m_timeInternal.m_counter);
            m_time.smoothed.m_fps = (float)1.0f / m_time.smoothed.m_dt;
            m_time.smoothed.m_gpu = (float)(m_timeInternal.m_gpuSum / (float)m_timeInternal.m_counter);
            m_time.smoothed.m_gpuWait = (float)(m_timeInternal.m_gpuWaitSum / (float)m_timeInternal.m_counter);

            m_timeInternal.reset();
        }
    }

    //--------------------------------------------------------------------------------------
    void Engine::toggleFullscreen()
    {
        if (m_renderer)
            m_renderer->SetFullscreen(!m_renderer->IsFullscreen());
    }

    static bool g_RunningOneFrame = false;

    //--------------------------------------------------------------------------------------
    void Engine::FlushLoading()
    {
        m_resourceManager->updateLoading();
    }

	//--------------------------------------------------------------------------------------
	void Engine::RunOneFrame()
	{
        if (g_RunningOneFrame)
            return;
        g_RunningOneFrame = true;

        VG_PROFILE_CPU("Engine");       

        updateDt();

        auto * input = Kernel::getInput();
        input->Update();

        if (input->IsKeyJustPressed(Key::V))
        {
            if (auto * renderer = GetRenderer())
            {
                if (auto * rendererOptions = renderer->GetOptions())
                {
                    auto vsync = rendererOptions->GetVSync();

                    for (uint i = 1; i < (uint)enumCount<gfx::VSync>(); ++i)
                    {
                        gfx::VSync next = (gfx::VSync)((((uint)vsync) + i) % (uint)enumCount<gfx::VSync>());
                        if (renderer->IsVSyncSupported(next))
                        {
                            rendererOptions->SetVSync(next);
                            break;
                        }
                    }
                }
            }
        }
        else if (input->IsKeyJustPressed(Key::H))
        {
            if (auto * renderer = GetRenderer())
            {
                if (auto * rendererOptions = renderer->GetOptions())
                {
                    auto hdr = rendererOptions->GetHDR();

                    for (uint i = 1; i < (uint)enumCount<gfx::HDR>(); ++i)
                    {
                        gfx::HDR next = (gfx::HDR)((((uint)hdr) + i) % (uint)enumCount<gfx::HDR>());
                        if (renderer->IsHDRSupported(next))
                        {
                            rendererOptions->SetHDR(next);
                            break;
                        }
                    }
                }
            }
        }

        m_resourceManager->updateLoading();

        const float scaledDeltaTime = m_time.m_scaledDeltaTime;
        const float realDeltaTime = m_time.m_realDeltaTime;

        auto getWorldDeltaTime = [=](IWorld * _world)
        {
            if (_world->IsPrefabWorld())
            {
                return realDeltaTime;
            }
            else
            {
                return scaledDeltaTime;
            }
        };

        const bool playing = isPlaying();
        const bool paused = playing && isPaused();

        // FixedUpdate all GameObjects and components
        {
            VG_PROFILE_CPU("FixedUpdate");
            for (IWorld * world : GetWorlds())
            {
                GameObject::Context gameObjectUpdateContext(playing, paused, getWorldDeltaTime(world), world);

                for (uint i = 0; i < world->GetSceneCount(BaseSceneType::Scene); ++i)
                {
                    const Scene * scene = (Scene *)world->GetScene(i, BaseSceneType::Scene);
                    GameObject * root = scene->getRoot();
                    if (root && asBool(UpdateFlags::FixedUpdate & root->getUpdateFlags()))
                        root->FixedUpdate(gameObjectUpdateContext);
                }
            }
        }

        // This will use all available threads for physics
        if (m_physics)
            m_physics->Update(scaledDeltaTime);

        if (m_game)
            m_game->Update(scaledDeltaTime);

        // Update
        {
            VG_PROFILE_CPU("Update");
            for (IWorld * world : GetWorlds())
            {
                GameObject::Context gameObjectUpdateContext(playing, paused, getWorldDeltaTime(world), world);

                for (uint i = 0; i < world->GetSceneCount(BaseSceneType::Scene); ++i)
                {
                    const Scene * scene = (Scene *)world->GetScene(i, BaseSceneType::Scene);
                    GameObject * root = scene->getRoot();
                    if (root && asBool(UpdateFlags::Update & root->getUpdateFlags()))
                        root->Update(gameObjectUpdateContext);
                }
            }
        }

        // Sync animations so the result can be used during LateUpdate & Render
        auto * scheduler = Kernel::getScheduler();
        {
            VG_PROFILE_CPU("Sync Animation");
            scheduler->Wait(getJobSync(EngineJobType::Animation));
        }

        // LateUpdate all GameObjects and components
        {
            VG_PROFILE_CPU("LateUpdate");
            for (IWorld * world : GetWorlds())
            {
                GameObject::Context gameObjectUpdateContext(playing, paused, getWorldDeltaTime(world), world);

                for (uint i = 0; i < world->GetSceneCount(BaseSceneType::Scene); ++i)
                {
                    const Scene * scene = (Scene *)world->GetScene(i, BaseSceneType::Scene);
                    GameObject * root = scene->getRoot();
                    if (root && asBool(UpdateFlags::LateUpdate & root->getUpdateFlags()))
                        root->LateUpdate(gameObjectUpdateContext);
                }
            }
        }

        #if VG_ENABLE_EDITOR
        if (m_editor)
            m_editor->RunOneFrame();
        #endif

        // This will use all available threads for culling then rendering scene
        if (m_renderer)
            m_renderer->RunOneFrame();

        if (m_startInPlayMode)
        {
            engine::IResourceManager * rm = GetResourceManager();
            if (!rm->HasResourceLoading())
            {
                Play();
                m_startInPlayMode = false;
            }
        }

        g_RunningOneFrame = false;
	}

    //--------------------------------------------------------------------------------------
    #if VG_ENABLE_EDITOR
    editor::IEditor * Engine::GetEditor() const
    {
        return m_editor;
    }
    #endif

	//--------------------------------------------------------------------------------------
	renderer::IRenderer * Engine::GetRenderer() const
	{
		return m_renderer;
	}

    //--------------------------------------------------------------------------------------
    physics::IPhysics * Engine::GetPhysics() const
    {
        return m_physics;
    }

    //--------------------------------------------------------------------------------------
    audio::IAudio * Engine::GetAudio() const
    {
        return m_audio;
    }

    //--------------------------------------------------------------------------------------
    IResourceManager * Engine::GetResourceManager() const
    {
        return m_resourceManager;
    }

    //--------------------------------------------------------------------------------------
    core::ISelection * Engine::GetSelection() const
    {
        return m_selection;
    }

    //--------------------------------------------------------------------------------------
    core::uint2 Engine::GetScreenSize() const
    {
        return GetRenderer()->GetBackbufferSize();
    }

    //--------------------------------------------------------------------------------------
    const Time & Engine::GetTime() const
    {
        return getTime();
    }

    //--------------------------------------------------------------------------------------
    bool Engine::IsPlaying() const
    {
        return isPlaying();
    }

    //--------------------------------------------------------------------------------------
    bool Engine::IsPaused() const
    {
        return isPaused();
    }

    //--------------------------------------------------------------------------------------
    void Engine::Play()
    {
        play();
    }

    //--------------------------------------------------------------------------------------
    void Engine::Stop()
    {
        stop();
    }

    //--------------------------------------------------------------------------------------
    void Engine::Pause()
    {
        pause();
    }

    //--------------------------------------------------------------------------------------
    void Engine::Resume()
    {
        resume();
    }

    //--------------------------------------------------------------------------------------
    void Engine::play()
    {
        VG_INFO("[Engine] Play");
        m_isPlaying = true;
        m_isPaused = false;

        m_time.m_enlapsedTimeSinceStartReal = 0.0f;
        m_time.m_enlapsedTimeSinceStartScaled = 0.0f;
        m_timeInternal.reset();

        // Detect joypads
        Kernel::getInput()->OnPlay();

        if (auto * mainWorld = GetMainWorld())
            mainWorld->OnPlay();
    }

    //--------------------------------------------------------------------------------------
    void Engine::pause()
    {
        VG_INFO("[Engine] Pause");
        VG_ASSERT(m_isPlaying);
        m_isPaused = true;
    }

    //--------------------------------------------------------------------------------------
    void Engine::resume()
    {
        VG_INFO("[Engine] Resume");
        VG_ASSERT(m_isPlaying && m_isPaused);
        m_isPaused = false;
    }

    //--------------------------------------------------------------------------------------
    void Engine::stop()
    {
        VG_INFO("[Engine] Stop");
        m_isPlaying = false;
        m_isPaused = false;

        if (auto * mainWorld = GetMainWorld())
            mainWorld->OnStop();
    }
}