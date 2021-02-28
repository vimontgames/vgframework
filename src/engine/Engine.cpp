#include "engine/Precomp.h"

#include "engine.h"

#include "core/Kernel.h"
#include "core/IProfiler.h"
#include "core/Timer/Timer.h"
#include "core/Plugin/Plugin.h"
#include "core/Scheduler/Scheduler.h"
#include "core/Entity/Entity.h"

#include "graphics/renderer/IRenderer.h"
#include "graphics/renderer/IView.h"
#include "graphics/driver/IDevice.h"

#include "engine/Input/Input.h"
#include "engine/Entity/FreeCam/FreeCamEntity.h"
#include "engine/Component/Camera/CameraComponent.h"

using namespace vg::core;
using namespace vg::engine;

#define VG_ENGINE_VERSION_MAJOR 0
#define VG_ENGINE_VERSION_MINOR 1

//--------------------------------------------------------------------------------------
IEngine * CreateNew()
{
	return new Engine();
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
	Engine::Engine()
	{

	}

	//--------------------------------------------------------------------------------------
	Engine::~Engine()
	{

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

		m_renderer = Plugin::create<graphics::renderer::IRenderer>("renderer", api);
		m_renderer->init(_params.renderer, _singletons);

        // Profiler has to be created by the renderer to be able to also profile the GPU
        Kernel::setProfiler(_singletons.profiler);

        // Register threads after profiler creation
        _singletons.scheduler->registerProfilerThreads();

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

        CameraComponent * cameraComponent = new CameraComponent();
        m_freeCam = new FreeCamEntity("FreeCam #0");
        m_freeCam->addComponent(cameraComponent);
        cameraComponent->setView(m_editorView);

        addEntity(m_freeCam);
        
        VG_SAFE_RELEASE(cameraComponent);
    }

    //--------------------------------------------------------------------------------------
    void Engine::destroyEditorView()
    {
        VG_SAFE_RELEASE(m_freeCam);
        VG_SAFE_RELEASE(m_editorView);
    }

	//--------------------------------------------------------------------------------------
	void Engine::deinit()
	{
        m_renderer->waitGPUIdle();

        destroyEditorView();
        destroyEntities();

        Kernel::setProfiler(nullptr);

        IScheduler * scheduler = Kernel::getScheduler();
        VG_SAFE_DELETE(scheduler);
        Kernel::setScheduler(nullptr);

        IInput * input = Kernel::getInput();
        VG_SAFE_DELETE(input);
        Kernel::setInput(nullptr);

		m_renderer->deinit();
		m_renderer->release();
	}

    //--------------------------------------------------------------------------------------
    void Engine::updateDt()
    {
        static Timer::Tick previous = 0;
        Timer::Tick current = Timer::getTick();
        if (previous != 0)
            m_dt = Timer::getEnlapsedTime(previous, current);
        previous = current;

        //VG_DEBUGPRINT("dt = %f ms\n", m_dt);

        // check profiler leak
        //static uint counter = 0;
        //if (counter == 1 || counter == 2)
        //    VG_PROFILE_TRIGGER();
        //counter++;
    }

    //--------------------------------------------------------------------------------------
    void Engine::destroyEntities()
    {
        for (uint i = 0; i < m_entities.size(); ++i)
            VG_SAFE_RELEASE(m_entities[i]);

        m_entities.clear();
    }

    //--------------------------------------------------------------------------------------
    void Engine::addEntity(core::Entity * _entity)
    {
        VG_ASSERT(_entity);
        _entity->addRef();
        m_entities.push_back(_entity);
    }

    //--------------------------------------------------------------------------------------
    void Engine::updateEntities(double _dt)
    {
        for (uint e = 0; e < m_entities.size(); ++e)
        {
            Entity * entity = m_entities[e];
            entity->update(_dt);
        }
    }

	//--------------------------------------------------------------------------------------
	void Engine::runOneFrame()
	{
        VG_PROFILE_FRAME("Main");
        VG_PROFILE_CPU("Engine");

        updateDt();

        Kernel::getInput()->update();

        updateEntities(m_dt);

        // test
        //((Scheduler*)Kernel::getScheduler())->test();

		m_renderer->runOneFrame(m_dt);
	}

	//--------------------------------------------------------------------------------------
	graphics::renderer::IRenderer * Engine::getRenderer() const
	{
		return m_renderer;
	}

    //--------------------------------------------------------------------------------------
    core::uint2 Engine::getScreenSize() const
    {
        return getRenderer()->getBackbufferSize();
    }
}