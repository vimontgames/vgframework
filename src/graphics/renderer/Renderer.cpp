#include "graphics/renderer/Precomp.h"
#include "renderer.h"

#include "core/Kernel.h"
#include "graphics/driver/device/device.h"
#include "graphics/driver/Shader/ShaderManager.h"
#include "graphics/driver/FrameGraph/FrameGraph.h"
#include "graphics/driver/Profiler/Profiler.h"
#include "graphics/renderer/Imgui/imguiAdapter.h"
#include "graphics/renderer/Pass/TestPass.h"
#include "graphics/renderer/Pass/ImguiPass.h"

#include "shaders/driver/driver.hlsl.h"

using namespace vg::core;
using namespace vg::graphics::driver;
using namespace vg::graphics::renderer;

//--------------------------------------------------------------------------------------
IRenderer * CreateNew()
{
	return new Renderer();
}

//--------------------------------------------------------------------------------------
namespace vg::graphics::renderer
{
	#define VG_RENDERER_VERSION_MAJOR 0
	#define VG_RENDERER_VERSION_MINOR 1

    #ifdef _WIN32
    //--------------------------------------------------------------------------------------
    LRESULT CALLBACK Renderer::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
            return true;

        return 0;
    }
    #endif

    //--------------------------------------------------------------------------------------
    core::IProfiler * Renderer::getProfilerInstance() const
    {
        return Kernel::getProfiler();
    }

	//--------------------------------------------------------------------------------------
	core::IPlugin::Version Renderer::getVersion() const
	{
		return { VG_RENDERER_VERSION_MAJOR, VG_RENDERER_VERSION_MINOR };
	}

	//--------------------------------------------------------------------------------------
	Renderer::Renderer() :
		m_device(*(new Device())),
		m_frameGraph(*(new FrameGraph()))
	{
        Kernel::setProfiler(new Profiler());
	}

	//--------------------------------------------------------------------------------------
	Renderer::~Renderer()
	{
        m_frameGraph.release();
		m_device.release();
        IProfiler * profiler = Kernel::getProfiler();
        VG_SAFE_DELETE(profiler);
	}

	//--------------------------------------------------------------------------------------
	void Renderer::init(const RendererParams & _params)
	{
        // Create device
		m_device.init(_params.device);

        // UI
        m_imgui = new ImguiAdapter(_params.device.window, m_device);

        // Register shaders
        auto * sm = ShaderManager::get();
        sm->registerHLSL(DriverHLSLDesc());

        // Create passes
        m_testPass = new TestPass();
        m_imguiPass = new ImguiPass();
	}

	//--------------------------------------------------------------------------------------
	void Renderer::deinit()
	{
        VG_SAFE_DELETE(m_testPass);
        VG_SAFE_DELETE(m_imguiPass);
        VG_SAFE_DELETE(m_imgui);

		m_device.deinit();
	}

	//--------------------------------------------------------------------------------------
	void Renderer::runOneFrame()
	{
        VG_PROFILE_CPU("Renderer");

		m_device.beginFrame();
		{
            m_imgui->beginFrame();

            // Framegraph
            {
                VG_PROFILE_CPU("Framegraph");

                m_frameGraph.import("Backbuffer", m_device.getBackbuffer());
                m_frameGraph.setGraphOutput("Backbuffer");

                m_frameGraph.addUserPass(m_testPass, "TestPass");
                m_frameGraph.addUserPass(m_imguiPass, "UIPass");

                m_frameGraph.setup();
                m_frameGraph.build();
                m_frameGraph.render();
            }
		}
		m_device.endFrame();
	}

	//--------------------------------------------------------------------------------------
	Texture * Renderer::getBackbuffer()
	{
		return m_device.getBackbuffer();
	}
}