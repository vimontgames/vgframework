#include "engine/Precomp.h"

#include "engine.h"

#include "core/Kernel.h"
#include "core/IProfiler.h"
#include "core/Timer/Timer.h"
#include "core/Plugin/Plugin.h"
#include "core/Scheduler/Scheduler.h"
#include "core/Entity/Entity.h"
#include "core/Object/Factory.h"
#include "core/Scene/Scene.h"
#include "core/Sector/Sector.h"

#include "graphics/renderer/IRenderer.h"
#include "graphics/renderer/IView.h"
#include "graphics/driver/IDevice.h"

#include "engine/Input/Input.h"
#include "engine/Resource/ResourceManager.h"
#include "engine/Entity/FreeCam/FreeCamEntity.h"
#include "engine/Component/Camera/CameraComponent.h"
#include "engine/Component/Mesh/MeshComponent.h"

#include "application/IProject.h"

using namespace vg::core;
using namespace vg::engine;

#define VG_ENGINE_VERSION_MAJOR 0
#define VG_ENGINE_VERSION_MINOR 12

// Avoid stripping code for classes from static lib
static Scene scene("",nullptr);
static Sector sector("", nullptr);

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

    //--------------------------------------------------------------------------------------
    bool Engine::load(IObject * _object)
    {
        return _object->loadFromFile("engine.ini");
    }

    //--------------------------------------------------------------------------------------
    bool Engine::save(IObject * _object)
    {
        return _object->saveToFile("engine.ini");
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

        _desc.registerPropertyHelper(Engine, m_projectPath, "Project folder", IProperty::Flags::IsFolder);

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
    core::IScene * Engine::getScene() const
    {
        return m_scene;
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

			case graphics::driver::API::DirectX12:
				api = "DX12";
				break;

			case graphics::driver::API::Vulkan:
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

		m_renderer = Plugin::create<graphics::renderer::IRenderer>("renderer", api);
		m_renderer->init(_params.renderer, _singletons);

        // Profiler has to be created by the renderer to be able to also profile the GPU
        Kernel::setProfiler(_singletons.profiler);

        // Register threads after profiler creation
        _singletons.scheduler->registerProfilerThreads();

        m_resourceManager = new ResourceManager("Resource Manager", this);

        registerClasses();

        createEditorView();
	}

    //--------------------------------------------------------------------------------------
    void Engine::createEditorView()
    {
        graphics::renderer::CreateViewParams params;
                                             params.offset = { 0,0 };
                                             params.size = getScreenSize();

        m_editorView = m_renderer->createView(params);
        m_renderer->setView(m_editorView);

        // use factor to create objects
        auto * factory = Kernel::getFactory();

        // create empty scene
        m_scene = (IScene*)CreateFactoryObject(Scene, "TestScene", this);
        m_scene->setName("Scene");

        // add root sector
        Sector * rootSector = (Sector*)CreateFactoryObject(Sector, "Root", m_scene);
        rootSector->setName("Root");
        m_scene->setRoot(rootSector);
        VG_SAFE_RELEASE(rootSector);


        // add camera entity
        auto * root = m_scene->getRoot();
        m_freeCam = new FreeCamEntity("FreeCam #0", root);
        auto * cameraComponent = (CameraComponent*)CreateFactoryObject(CameraComponent, "", m_freeCam);
        m_freeCam->addComponent(cameraComponent);
        cameraComponent->setView(m_editorView, root);
        root->addEntity(m_freeCam);
        VG_SAFE_RELEASE(cameraComponent);
        VG_SAFE_RELEASE(m_freeCam);

        auto addMeshToScene = [=](const string & _name, const string & _path, const float4 _position)
        {
            auto * meshEntity = (Entity*)CreateFactoryObject(Entity, _name.c_str(), root);
            auto * meshComponent = (MeshComponent*)CreateFactoryObject(MeshComponent, "", meshEntity);
            meshComponent->getMeshResource().setPath(_path);
            meshEntity->addComponent(meshComponent);
            root->addEntity(meshEntity);

            const float4x4 m =
            {
                float4(1.0f, 0.0f, 0.0f, (0.0f)),
                float4(0.0f, 1.0f, 0.0f, (0.0f)),
                float4(0.0f, 0.0f, 1.0f, (0.0f)),
                _position,
            };
            meshEntity->SetWorldMatrix(m);

            VG_SAFE_RELEASE(meshComponent);
            VG_SAFE_RELEASE(meshEntity);
        };

#if 1
        //addMeshToScene("Box", "data/Models/matIDBox/matIDBox.fbx", float4(80.0f, 0.0f, 10.0f, 1.0f));
        addMeshToScene("3DScanMan001", "data/Models/human/human.fbx", float4(-1.0f, 0.0f, 0.0f, 1.0f));
        addMeshToScene("3DScanMan016", "data/Models/3DScan_Man_016/3DScan_Man_016.FBX", float4(0.0f, 0.0f, 0.0f, 1.0f));
        addMeshToScene("Jess", "data/Models/jess/jess.fbx", float4(+1.0f, 0.0f, 00.0f, 1.0f));
        //addMeshToScene("Floor", "data/Models/floor/floor.fbx", float4(0.0f, 0.0f, -10.0f, 1.0f));
#else
        // add a few entities with mesh
        for (int j = 0; j < 3; ++j)
        {
            for (int i = 0; i < 3; ++i)
            {
                auto * meshEntity = (Entity*)CreateFactoryObject(Entity, "", root);
                auto * meshComponent = (MeshComponent*)CreateFactoryObject(MeshComponent, "", meshEntity);

                meshComponent->getMeshResource().setPath("data/Models/Teapot.fbx");
                meshEntity->addComponent(meshComponent);

                root->addEntity(meshEntity);

                const float x = (i - 1) * 64.0f;
                const float y = (j - 1) * 64.0f;

                const float4x4 m =
                {
                    float4(1.0f, 0.0f, 0.0f, (0.0f)),
                    float4(0.0f, 1.0f, 0.0f, (0.0f)),
                    float4(0.0f, 0.0f, 1.0f, (0.0f)),
                    float4(   x,    y, 0.0f, (1.0f)),
                };

                meshEntity->SetWorldMatrix(m);

                VG_SAFE_RELEASE(meshComponent);
                VG_SAFE_RELEASE(meshEntity);
            }
        }
#endif
    }

    //--------------------------------------------------------------------------------------
    void Engine::destroyEditorView()
    {
        VG_SAFE_RELEASE(m_freeCam);
        VG_SAFE_RELEASE(m_scene);
        VG_SAFE_RELEASE(m_editorView);
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
    void Engine::updateEntities(Sector * _sector, double _dt)
    {
        const auto & entities = _sector->getEntities();
        for (uint e = 0; e < entities.size(); ++e)
        {
            Entity * entity = entities[e];
            entity->update(_dt);
        }

        const auto & sectors = _sector->getChildSectors();
        for (uint s = 0; s < sectors.size(); ++s)
        {
            Sector * sector = sectors[s];
            updateEntities(sector, _dt);
        }
    }

	//--------------------------------------------------------------------------------------
	void Engine::runOneFrame()
	{
        VG_PROFILE_FRAME("Main");
        VG_PROFILE_CPU("Engine");

        updateDt();

        Kernel::getInput()->update();

        m_resourceManager->updateLoading();

        Sector * root = (Sector*)m_scene->getRoot();
        if (root)
            updateEntities(root, m_dt);
        
		m_renderer->runOneFrame(m_dt);
	}

	//--------------------------------------------------------------------------------------
	graphics::renderer::IRenderer * Engine::GetRenderer() const
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
}