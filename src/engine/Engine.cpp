#include "engine/Precomp.h"

#include "engine.h"

#include "core/Kernel.h"
#include "core/IProfiler.h"
#include "core/Universe/Universe.h"
#include "core/Timer/Timer.h"
#include "core/Plugin/Plugin.h"
#include "core/Scheduler/Scheduler.h"
#include "core/Object/Factory.h"
#include "core/Scene/Scene.h"
#include "core/GameObject/GameObject.h"

#include "gfx/IView.h"
#include "gfx/IDevice.h"

#include "renderer/IRenderer.h"

#include "engine/Input/Input.h"
#include "engine/Resource/ResourceManager.h"
#include "engine/Component/Camera/CameraComponent.h"
#include "engine/Behaviour/FreeCam/FreeCamBehaviour.h"
#include "engine/Component/Mesh/MeshComponent.h"

//#include "editor/IEditor.h"

#include "application/IProject.h"

#if !VG_ENABLE_INLINE
#include "Engine.inl"
#endif

using namespace vg::core;
using namespace vg::engine;

#define VG_ENGINE_VERSION_MAJOR 0
#define VG_ENGINE_VERSION_MINOR 15

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
	IPlugin::Version Engine::getVersion() const
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

                    case VK_F6:
                        if (m_renderer)
                            m_renderer->updateShaders();
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
    bool Engine::registerClasses()
    {
        core::IFactory * factory = Kernel::getFactory();

        // Register classes to auto-register the "Engine" module
        AutoRegisterClassInfo::registerClasses(*factory);

        if (core::IClassDesc * desc = factory->registerClassSingletonHelper(Engine, "Engine", IClassDesc::Flags::None))
            registerProperties(*desc);

        load(this);
        loadProject(m_projectPath);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Engine::unregisterClasses()
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

        //_desc.registerCallbackHelper(Engine, createProject, "Create Project", IProperty::Flags::None);
        //_desc.registerCallbackHelper(Engine, loadProject, "Load", IProperty::Flags::None);
        //_desc.registerCallbackHelper(Engine, saveProject, "Save", IProperty::Flags::None);
        //_desc.registerProperty("Project", (IObject**)(&((Engine*)(nullptr))->m_project), "Project", IProperty::Flags::None);
        //_desc.registerPropertyHelper(Engine, m_project, "Project", IProperty::Flags::None);
        //_desc.registerProperty("m_project", (IResource**)(&((Engine*)(nullptr))->m_project), "Project", IProperty::Flags::None);

        _desc.registerPropertyHelperEx(Engine, m_projectPath, "Project folder", IProperty::Flags::IsFolder);

        //_desc.registerCallbackHelper(Engine, createProject, "Create Project", IProperty::Flags::None);
        
        _desc.registerCallbackHelper(DisplayOptions, load, "Load", IProperty::Flags::None);
        _desc.registerCallbackHelper(DisplayOptions, save, "Save", IProperty::Flags::SameLine);

        return true;
    }

    //--------------------------------------------------------------------------------------
    void Engine::onPropertyChanged(const IProperty & _prop)
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
	void Engine::init(const EngineParams & _params, Singletons & _singletons)
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
        _singletons.scheduler = new Scheduler();
        Kernel::setScheduler(_singletons.scheduler);

        _singletons.input = new Input(_params.renderer.device.window);
        Kernel::setInput(_singletons.input);

        _singletons.factory = new Factory();
        Kernel::setFactory(_singletons.factory);

        // Load Renderer DLL
		m_renderer = Plugin::create<renderer::IRenderer>("renderer", api);
		m_renderer->init(_params.renderer, _singletons);

        // Profiler has to be created by the renderer to be able to also profile the GPU
        Kernel::setProfiler(_singletons.profiler);
        _singletons.scheduler->RegisterCurrentThread("Main");

        // Register worker threads, it will be useful to get worker thread names in profiler
        _singletons.scheduler->RegisterWorkerThreads();

        m_resourceManager = new ResourceManager("Resource Manager", this);

        registerClasses();

        // Load Editor DLL
        //m_editor = Plugin::create<editor::IEditor>("editor");
        //m_editor->Init();

        // Create main view
        m_mainView = m_renderer->CreateMainView(getScreenSize());

        createEditorScene();
	}

    //--------------------------------------------------------------------------------------
    void Engine::createEditorScene()
    {
        // use factor to create objects
        auto* factory = Kernel::getFactory();

        // create default universe
        m_universe = (Universe*)CreateFactoryObject(Universe, "DefaultUniverse", this);

        Scene * editor = (Scene *)CreateFactoryObject(Scene, "Editor", m_universe);
        m_universe->addScene(editor);
        editor->release();

        GameObject * rootGameObject = (GameObject *)CreateFactoryObject(GameObject, "Root", editor);
        editor->SetRoot(rootGameObject);
        rootGameObject->release();

        // add Camera GameObject
        GameObject * editorCameraGameObject = new GameObject("Editor Camera", rootGameObject);

        // add Camera component
        auto * cameraComponent = editorCameraGameObject->addComponent<CameraComponent>("Camera");
        cameraComponent->setViewType(gfx::ViewType::Editor);

        // add FreeCam behaviour
        auto * freeCamComponent = editorCameraGameObject->addComponent<FreeCamBehaviour>("FreeCam");
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

        unregisterClasses();

        Kernel::setProfiler(nullptr);

        IScheduler * scheduler = Kernel::getScheduler();
        VG_SAFE_DELETE(scheduler);
        Kernel::setScheduler(nullptr);

        IInput * input = Kernel::getInput();
        VG_SAFE_DELETE(input);
        Kernel::setInput(nullptr);

        IFactory * factory = Kernel::getFactory();

        //m_editor->Deinit();
        //m_editor->Release();

		m_renderer->deinit();
		m_renderer->release();

        VG_SAFE_DELETE(m_resourceManager);

        VG_SAFE_DELETE(factory);
        Kernel::setFactory(nullptr);
	}

    //--------------------------------------------------------------------------------------
    void Engine::updateDt()
    {
        static Timer::Tick previous = 0;
        Timer::Tick current = Timer::getTick();
        if (previous != 0)
            m_dt = Timer::getEnlapsedTime(previous, current);
        previous = current;
    }

	//--------------------------------------------------------------------------------------
	void Engine::RunOneFrame()
	{
        VG_PROFILE_FRAME("Main");
        VG_PROFILE_CPU("Engine");

        updateDt();

        Kernel::getInput()->Update();

        m_resourceManager->updateLoading();

        if (m_universe)
        {
            const uint sceneCount = m_universe->getSceneCount();
            for (uint i = 0; i < sceneCount; ++i)
            {
                Scene * scene = (Scene*)m_universe->getScene(i);
                GameObject * root = scene->getRoot();
                if (root)
                    root->Update(m_dt);
            }

            m_renderer->runOneFrame(m_dt);
        }
	}

	//--------------------------------------------------------------------------------------
	renderer::IRenderer * Engine::GetRenderer() const
	{
		return m_renderer;
	}

    //--------------------------------------------------------------------------------------
    IResourceManager * Engine::GetResourceManager() const
    {
        return m_resourceManager;
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
    void Engine::Pause()
    {
        pause();
    }

    //--------------------------------------------------------------------------------------
    void Engine::Stop()
    {
        stop();
    }
}