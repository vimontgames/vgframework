#include "engine/Precomp.h"

#include "engine.h"

#include "core/Kernel.h"
#include "core/IProfiler.h"
#include "core/Timer/Timer.h"
#include "core/Plugin/Plugin.h"
#include "graphics/renderer/IRenderer.h"
#include "graphics/driver/IDevice.h"

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
	void Engine::init(const EngineParams & _params)
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

		m_renderer = Plugin::create<graphics::renderer::IRenderer>("renderer", api);
		m_renderer->init(_params.renderer);

        Kernel::setProfiler(m_renderer->getProfilerInstance());
	}

	//--------------------------------------------------------------------------------------
	void Engine::deinit()
	{
        Kernel::setProfiler(nullptr);

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
    }

	//--------------------------------------------------------------------------------------
	void Engine::runOneFrame()
	{
        VG_PROFILE_FRAME("Main");
        VG_PROFILE_CPU("Engine");

        updateDt();

		m_renderer->runOneFrame(m_dt);
	}

	//--------------------------------------------------------------------------------------
	graphics::renderer::IRenderer * Engine::getRenderer() const
	{
		return m_renderer;
	}
}