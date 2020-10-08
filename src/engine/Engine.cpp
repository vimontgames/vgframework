#include "engine/Precomp.h"

#include "engine.h"
#include "graphics/renderer/IRenderer.h"
#include "graphics/driver/IDevice.h"
#include "core/Plugin/Plugin.h"

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
		string api;
		switch (_params.renderer.device.api)
		{
			default:
				VG_ASSERT(false);
				break;

			case graphics::driver::API::DirectX12:
				api = "dx12";
				break;

			case graphics::driver::API::Vulkan:
				api = "vulkan";
				break;
		}

		const auto & name = asString(_params.renderer.device.api);

		m_renderer = Plugin::create<graphics::renderer::IRenderer>("renderer", api);
		m_renderer->init(_params.renderer);
	}

	//--------------------------------------------------------------------------------------
	void Engine::deinit()
	{
		m_renderer->deinit();
		m_renderer->release();
	}

	//--------------------------------------------------------------------------------------
	void Engine::runOneFrame()
	{
		m_renderer->runOneFrame();
	}

	//--------------------------------------------------------------------------------------
	graphics::renderer::IRenderer * Engine::getRenderer() const
	{
		return m_renderer;
	}
}