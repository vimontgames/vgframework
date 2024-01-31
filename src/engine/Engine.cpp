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

#include "gfx/IView.h"
#include "gfx/IDevice.h"

#include "renderer/IRenderer.h"

#include "physics/IPhysics.h"

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

#include "application/IProject.h"

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
                    m_renderer->resize(width, height);
                }
                break;

            case WM_PAINT:
                RunOneFrame();
                break;

            case WM_KEYDOWN:
                switch (wParam)
                {
                    case VK_F1:
                        VG_PROFILE_TRIGGER();   // Start/Stop capture
                        break;

                    case VK_ESCAPE:
                    {
                        if (IsPlaying())
                            Stop();

                        if (m_renderer && m_renderer->IsFullscreen())
                            m_renderer->SetFullscreen(false);
                    }
                    break;;

                    case VK_F5:
                    {
                        if (GetKeyState(VK_SHIFT) & 0x8000)
                        {
                            if (IsPlaying())
                            {
                                Stop();
                                Play();
                            }
                        }
                        else
                        {
                            if (!IsPlaying())
                                Play();
                        }
                    }
                    break;

                    case VK_F6:
                        if (m_renderer)
                            m_renderer->updateShaders();
                        break;

                    case VK_F7:
                        if (m_resourceManager)
                            m_resourceManager->UpdateResources();
                        break;

                    case VK_PAUSE:
                        if (isPaused())
                            Resume();
                        else
                            Pause();
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
        IEngine(_name, _parent)
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

        // Register classes to auto-register the "Engine" module
        AutoRegisterClassInfo::registerClasses(*factory);

        if (core::IClassDesc * desc = factory->registerPlugin(Engine, "Engine"))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Engine::UnregisterClasses()
    {
        IFactory * factory = Kernel::getFactory();
        return AutoRegisterClassInfo::unregisterClasses(*factory);
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
    bool Engine::loadProject(const string & _path)
    {
        if (_path.length() > 0)
        {
            auto folderNameBegin = _path.find_last_of('/');
            string projectDllName = _path + _path.substr(folderNameBegin);

            m_project = Plugin::create<IProject>(projectDllName);

            if (nullptr != m_project)
                m_project->init(*this, Kernel::getSingletons());
        }
        return nullptr != m_project;
    }

    //--------------------------------------------------------------------------------------
    bool Engine::unloadProject()
    {
        if (m_project)
        {
            m_project->deinit();
            VG_SAFE_RELEASE(m_project);
            m_project = nullptr;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    IProject * Engine::getProject() const
    {
        return m_project;
    }

    //--------------------------------------------------------------------------------------
    core::IWorld * Engine::getCurrentWorld() const
    {
        if (m_worldResource)
        {
            core::IObject * obj = m_worldResource->getObject();
            VG_ASSERT(obj == nullptr || dynamic_cast<WorldResourceData *>(obj));
            if (obj)
                return ((WorldResourceData *)obj)->getWorld();
        }

        return nullptr;
    }

	//--------------------------------------------------------------------------------------
	void Engine::init(const EngineCreationParams & _params, Singletons & _singletons)
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

        m_resourceManager = new ResourceManager("Resource Manager", this);

        RegisterClasses();

        // Load engine options
        EngineOptions * engineOptions = new EngineOptions("EngineOptions", this);  

        // Load project DLL
        loadProject(engineOptions->GetProjectPath());

        // Load Editor DLL
        m_editor = Plugin::create<editor::IEditor>("editor");
        m_editor->Init(_singletons);

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
    void Engine::onResourceLoaded(core::IResource * _resource)
    {
        if (_resource == m_worldResource)
        {
            VG_ASSERT(dynamic_cast<WorldResource*>(_resource));
            WorldResource * worldRes = (WorldResource *)_resource;
            auto world = worldRes->GetWorld();

            auto & editorViews = m_renderer->GetViews(gfx::ViewTarget::Editor);
            for (auto view : editorViews)
            {
                if (view)
                    view->SetWorld(world);
            }

            auto & gameViews = m_renderer->GetViews(gfx::ViewTarget::Game);
            for (auto view : gameViews)
            {
                if (view)
                    view->SetWorld(world);
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
        IObject * world = m_worldResource->getObject();
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
        IObject * world = m_worldResource->getObject();
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
    IEngineOptions * Engine::GetOptions()
    {
        return EngineOptions::get();
    }

	//--------------------------------------------------------------------------------------
	void Engine::deinit()
	{
        m_renderer->waitGPUIdle();
        m_resourceManager->flushPendingLoading();

        unloadProject();

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

        m_editor->Deinit();
        m_editor->Release();

        // ~dtor time is too late to unload world resource we have to use ptr so as to do it manually before the ResourceMananger shutdowns
        VG_SAFE_RELEASE(m_worldResource);

        // Resource Manager should be deleted before renderer because the shared resource must be released to avoid GPU memory leak checked in gfx::Device deinit
        VG_SAFE_DELETE(m_resourceManager);

        m_physics->Deinit();
        m_physics->release();

		m_renderer->deinit();
		m_renderer->release();

        VG_SAFE_DELETE(factory);
        Kernel::setFactory(nullptr);

        ILogger * logger = Kernel::getLogger();
        VG_SAFE_DELETE(logger);
        Kernel::setLogger(nullptr);

        VG_SAFE_RELEASE(m_selection);
        Kernel::setSelection(nullptr);
	}

    //--------------------------------------------------------------------------------------
    void Engine::Quit()
    {
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
        static Timer::Tick previous = 0;
        Timer::Tick current = Timer::getTick();
        if (previous != 0)
            m_time.m_dt = (float)(Timer::getEnlapsedTime(previous, current) * 0.001f); // use _dt in seconds for computations, not milliseconds
        previous = current;
    }

    //--------------------------------------------------------------------------------------
    void Engine::toggleFullscreen()
    {
        if (m_renderer)
            m_renderer->SetFullscreen(!m_renderer->IsFullscreen());
    }

    static bool g_RunningOneFrame = false;

	//--------------------------------------------------------------------------------------
	void Engine::RunOneFrame()
	{
        if (g_RunningOneFrame)
            return;
        g_RunningOneFrame = true;

        VG_PROFILE_CPU("Engine");       

        updateDt();

        Kernel::getInput()->Update();

        m_resourceManager->updateLoading();

        float dt = m_time.m_dt;

        IWorld * world = getCurrentWorld();
        if (world)
        {
            // FixedUpdate all GameObjects and components
            VG_PROFILE_CPU("FixedUpdate");
            const uint sceneCount = world->GetSceneCount(BaseSceneType::Scene);
            for (uint i = 0; i < sceneCount; ++i)
            {
                Scene * scene = (Scene *)world->GetScene(i, BaseSceneType::Scene);
                GameObject * root = scene->getRoot();
                if (root && asBool(UpdateFlags::FixedUpdate & root->getUpdateFlags()))
                    root->FixedUpdate(dt);
            }
        }

        // This will use all available threads for physics
        m_physics->RunOneFrame(m_time.m_dt);

        if (world)
        {
            // Update all GameObjects and components
            VG_PROFILE_CPU("Update");
            const uint sceneCount = world->GetSceneCount(BaseSceneType::Scene);
            for (uint i = 0; i < sceneCount; ++i)
            {
                Scene * scene = (Scene *)world->GetScene(i, BaseSceneType::Scene);
                GameObject * root = scene->getRoot();
                if (root && asBool(UpdateFlags::Update & root->getUpdateFlags()))
                    root->Update(dt);
            }
        }

        // Sync animations so the result can be used during LateUpdate & Render
        auto * scheduler = Kernel::getScheduler();
        {
            VG_PROFILE_CPU("Sync Animation");
            scheduler->Wait(getJobSync(JobSync::Animation));
        }

        if (world)
        {
            // LateUpdate all GameObjects and components
            VG_PROFILE_CPU("LateUpdate");
            const uint sceneCount = world->GetSceneCount(BaseSceneType::Scene);
            for (uint i = 0; i < sceneCount; ++i)
            {
                Scene * scene = (Scene *)world->GetScene(i, BaseSceneType::Scene);
                GameObject * root = scene->getRoot();
                if (root && asBool(UpdateFlags::LateUpdate & root->getUpdateFlags()))
                    root->LateUpdate(dt);
            }
        }

        if (m_editor)
            m_editor->RunOneFrame(m_time.m_dt);

        // This will use all available threads for culling then rendering scene (TODO)
        m_renderer->runOneFrame(m_time.m_dt);

        g_RunningOneFrame = false;
	}

    //--------------------------------------------------------------------------------------
    editor::IEditor * Engine::GetEditor() const
    {
        return m_editor;
    }

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
    core::uint2 Engine::getScreenSize() const
    {
        return GetRenderer()->getBackbufferSize();
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

        // Detect joypads
        Kernel::getInput()->OnPlay();

        IWorld * world = getCurrentWorld();
        if (nullptr != world)
        {
            for (uint i = 0; i < world->GetSceneCount(BaseSceneType::Scene); ++i)
            {
                const IBaseScene * scene = world->GetScene(i, BaseSceneType::Scene);
                if (nullptr != scene)
                {
                    IObject * root = scene->GetRoot();
                    if (nullptr != root)
                        root->OnPlay();
                }
            }
        }

        // Warmup physics
        if (m_physics)
            m_physics->OnPlay();
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

        IWorld * world = getCurrentWorld();
        if (nullptr != world)
        {
            for (uint i = 0; i < world->GetSceneCount(BaseSceneType::Scene); ++i)
            {
                const IBaseScene * scene = world->GetScene(i, BaseSceneType::Scene);
                if (nullptr != scene)
                {
                    IObject * root = scene->GetRoot();
                    if (nullptr != root)
                        root->OnStop();
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    const Time & Engine::GetTime() const
    {
        return getTime();
    }
}