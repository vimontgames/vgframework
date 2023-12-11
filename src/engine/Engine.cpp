#include "engine/Precomp.h"

#include "engine.h"

#include "core/Kernel.h"
#include "core/IProfiler.h"
#include "core/Universe/Universe.h"
#include "core/Timer/Timer.h"
#include "core/Plugin/Plugin.h"
#include "core/Logger/Logger.h"
#include "core/Scheduler/Scheduler.h"
#include "core/Object/Factory.h"
#include "core/Scene/Scene.h"
#include "core/GameObject/GameObject.h"

#include "gfx/IView.h"
#include "gfx/IDevice.h"

#include "renderer/IRenderer.h"

#include "physics/IPhysics.h"

#include "engine/Input/Input.h"
#include "engine/Resource/ResourceManager.h"
#include "engine/Selection/Selection.h"
#include "engine/Component/Camera/CameraComponent.h"
#include "engine/Behaviour/FreeCam/FreeCamBehaviour.h"
#include "engine/Component/Mesh/MeshComponent.h"

#include "editor/IEditor.h"

#include "application/IProject.h"

#if !VG_ENABLE_INLINE
#include "Engine.inl"
#endif

using namespace vg::core;
using namespace vg::engine;

#define VG_ENGINE_VERSION_MAJOR 0
#define VG_ENGINE_VERSION_MINOR 1

// Avoid stripping code for classes from static lib
static Universe universe("", nullptr);
static Scene scene("", nullptr);
static GameObject gameObject("", nullptr);

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
            case WM_SIZE:
                if (m_renderer)
                {
                    const uint width = uint(lParam & 0xFFFF);
                    const uint height = uint(lParam >> 16);
                    m_renderer->resize(width, height);
                }
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

        load(this);
        loadProject(m_projectPath);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Engine::UnregisterClasses()
    {
        IFactory * factory = Kernel::getFactory();
        return AutoRegisterClassInfo::unregisterClasses(*factory);
    }

    static const char * filename = "Engine.xml";

    //--------------------------------------------------------------------------------------
    bool Engine::load(IObject * _object)
    {
        const auto * factory = Kernel::getFactory();
        return factory->loadFromXML(_object, filename);
    }

    //--------------------------------------------------------------------------------------
    bool Engine::save(IObject * _object)
    {
        const auto * factory = Kernel::getFactory();
        return factory->saveToXML(_object, filename);
    }

    //--------------------------------------------------------------------------------------
    bool Engine::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        //registerPropertyCallback(Engine, createProject, "Create Project");
        //registerPropertyCallback(Engine, loadProject, "Load");
        //registerPropertyCallback(Engine, saveProject, "Save");
        //registerProperty("Project", (IObject**)(&((Engine*)(nullptr))->m_project), "Project");
        //registerProperty(Engine, m_project, "Project", IProperty::Flags::None);
        //registerProperty("m_project", (IResource**)(&((Engine*)(nullptr))->m_project), "Project");

        registerPropertyEx(Engine, m_projectPath, "Project folder", IProperty::Flags::IsFolder);

        //registerPropertyCallback(Engine, createProject, "Create Project");
        
        //registerPropertyCallbackEx(Engine, load, "Load", IProperty::Flags::None);
        //registerPropertyCallbackEx(Engine, save, "Save", IProperty::Flags::SameLine);

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
    core::IUniverse * Engine::getCurrentUniverse() const
    {
        return m_universe;
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

        // Load Editor DLL
        m_editor = Plugin::create<editor::IEditor>("editor");
        m_editor->Init(_singletons);

        // Create main view
        auto mainViewParams = gfx::CreateViewParams(gfx::ViewTarget::Backbuffer, getScreenSize());
        m_mainView = m_renderer->CreateView(mainViewParams, "MainView");

        createEditorScene();
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
    void Engine::createEditorScene()
    {
        // use factor to create objects
        auto* factory = Kernel::getFactory();

        // create default universe
        m_universe = (Universe*)CreateFactoryObject(Universe, "DefaultUniverse", this);

        Scene * editor = (Scene *)CreateFactoryObject(Scene, "Editor", m_universe);
        m_universe->AddScene(editor);
        editor->release();

        GameObject * rootGameObject = (GameObject *)CreateFactoryObject(GameObject, "Root", editor);
        editor->SetRoot(rootGameObject);
        rootGameObject->release();

        // add Camera GameObject
        GameObject * editorCameraGameObject = new GameObject("Editor Camera", rootGameObject);

        // add Camera component
        auto * cameraComponent = editorCameraGameObject->AddComponentByType<CameraComponent>("Camera");
        cameraComponent->setViewTarget(gfx::ViewTarget::Editor);

        // add FreeCam behaviour
        auto * freeCamComponent = editorCameraGameObject->AddComponentByType<FreeCamBehaviour>("FreeCam");
        editorCameraGameObject->setWorldMatrix(float4x4
        (
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, -8.0f, 2.0f, 1.0f
        ));

        // Add Camera GameObject
        rootGameObject->AddChild(editorCameraGameObject);
        VG_SAFE_RELEASE(editorCameraGameObject);
    }

    //--------------------------------------------------------------------------------------
    void Engine::destroyEditorView()
    {
        VG_SAFE_RELEASE(m_universe);
        VG_SAFE_RELEASE(m_mainView);
    }

	//--------------------------------------------------------------------------------------
	void Engine::deinit()
	{
        m_renderer->waitGPUIdle();
        m_resourceManager->flushPendingLoading();

        destroyEditorView();
        unloadProject();

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

	//--------------------------------------------------------------------------------------
	void Engine::RunOneFrame()
	{
        VG_PROFILE_CPU("Engine");

        updateDt();

        Kernel::getInput()->Update();

        m_resourceManager->updateLoading();

        float dt = this->isPlaying() && !this->isPaused() ? m_time.m_dt : 0.0f;

        if (m_universe)
        {
            // Update all GameObjects and components
            {
                VG_PROFILE_CPU("FixedUpdate");
                const uint sceneCount = m_universe->GetSceneCount();
                for (uint i = 0; i < sceneCount; ++i)
                {
                    Scene * scene = (Scene *)m_universe->GetScene(i);
                    GameObject * root = scene->getRoot();
                    if (root && asBool(UpdateFlags::FixedUpdate & root->getUpdateFlags()))
                        root->FixedUpdate(dt);
                }
            }

            // This will use all available threads for physics
            m_physics->RunOneFrame(m_time.m_dt);

            // Update all GameObjects and components
            {
                VG_PROFILE_CPU("Update");
                const uint sceneCount = m_universe->GetSceneCount();
                for (uint i = 0; i < sceneCount; ++i)
                {
                    Scene * scene = (Scene *)m_universe->GetScene(i);
                    GameObject * root = scene->getRoot();
                    if (root && asBool(UpdateFlags::Update & root->getUpdateFlags()))
                        root->Update(dt);
                }
            }

            // Update all GameObjects and components
            {
                VG_PROFILE_CPU("LateUpdate");
                const uint sceneCount = m_universe->GetSceneCount();
                for (uint i = 0; i < sceneCount; ++i)
                {
                    Scene * scene = (Scene *)m_universe->GetScene(i);
                    GameObject * root = scene->getRoot();
                    if (root && asBool(UpdateFlags::LateUpdate & root->getUpdateFlags()))
                        root->LateUpdate(dt);
                }
            }

            // This will use all available threads for culling then rendering scene (TODO)
            m_renderer->runOneFrame(m_time.m_dt);
        }
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
    gfx::IView * Engine::getMainView() const
    {
        return m_mainView;
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

        if (nullptr != m_universe)
        {
            for (uint i = 0; i < m_universe->GetSceneCount(); ++i)
            {
                const IScene * scene = m_universe->GetScene(i);
                if (nullptr != scene)
                {
                    IObject * root = scene->GetRoot();
                    if (nullptr != root)
                        root->OnPlay();
                }
            }
        }
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

        if (nullptr != m_universe)
        {
            for (uint i = 0; i < m_universe->GetSceneCount(); ++i)
            {
                const IScene * scene = m_universe->GetScene(i);
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