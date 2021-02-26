#include "graphics/renderer/Precomp.h"
#include "renderer.h"

#include "core/Kernel.h"
#include "graphics/driver/device/device.h"
#include "graphics/driver/Shader/ShaderManager.h"
#include "graphics/driver/FrameGraph/FrameGraph.h"
#include "graphics/driver/Profiler/Profiler.h"
#include "graphics/renderer/Imgui/imguiAdapter.h"
#include "graphics/renderer/Pass/BackgroundPass.h"
#include "graphics/renderer/Pass/TestPass.h"
#include "graphics/renderer/Pass/PostProcessPass.h"
#include "graphics/renderer/Pass/ImguiPass.h"

#include "shaders/driver/driver.hlsl.h"
#include "Shaders/background/background.hlsl.h"

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
        // Profiler instance has to be created by the graphic engine so as to profile the GPU too
        Kernel::setProfiler(new Profiler());
	}

	//--------------------------------------------------------------------------------------
	Renderer::~Renderer()
	{
		m_device.release();
        IProfiler * profiler = Kernel::getProfiler();
        VG_SAFE_DELETE(profiler);
	}

	//--------------------------------------------------------------------------------------
	void Renderer::init(const RendererParams & _params, core::Singletons & _singletons)
	{
        // Singletons created by the renderer
        _singletons.profiler = Kernel::getProfiler();

        // Singletons used by the renderer
        Kernel::setScheduler(_singletons.scheduler);

        // Create device
		m_device.init(_params.device);

        // UI
        m_imgui = new ImguiAdapter(_params.device.window, m_device);

        registerShaders();

        // Create passes
        m_backgroundPass = new BackgroundPass();
        m_testPass = new TestPass();
        m_postProcessPass = new PostProcessPass();
        m_imguiPass = new ImguiPass();
	}

    //--------------------------------------------------------------------------------------
    void Renderer::registerShaders()
    {
        auto * sm = ShaderManager::get();
        sm->registerHLSL(DriverHLSLDesc());
        sm->registerHLSL(BackgroundHLSLDesc());

        sm->update();
    }

	//--------------------------------------------------------------------------------------
	void Renderer::deinit()
	{
        VG_SAFE_DELETE(m_backgroundPass);
        VG_SAFE_DELETE(m_testPass);
        VG_SAFE_DELETE(m_postProcessPass);
        VG_SAFE_DELETE(m_imguiPass);
        VG_SAFE_DELETE(m_imgui);
        m_frameGraph.release();

		m_device.deinit();
	}

    //--------------------------------------------------------------------------------------
    void Renderer::resize(core::uint _width, core::uint _height)
    {
        m_device.waitGPUIdle();
        m_device.resize(_width, _height);
        m_frameGraph.destroyTransientResources();
    }

    //--------------------------------------------------------------------------------------
    void Renderer::updateShaders()
    {
        m_device.getShaderManager()->update();
    }

	//--------------------------------------------------------------------------------------
	void Renderer::runOneFrame(double _dt)
	{
        if (m_device.isMinimized())
        {
            Sleep(1);
            return;
        }

        VG_PROFILE_CPU("Renderer");

		m_device.beginFrame();
		{
            m_imgui->beginFrame();

            // Framegraph
            {
                VG_PROFILE_CPU("Framegraph");

                m_frameGraph.importRenderTarget("Backbuffer", m_device.getBackbuffer(), float4(0,0,0,0), FrameGraph::Resource::InitState::Clear);
                m_frameGraph.setGraphOutput("Backbuffer");

                m_frameGraph.addUserPass(m_backgroundPass, "BackgroundPass");
                m_frameGraph.addUserPass(m_testPass, "TestPass");
                m_frameGraph.addUserPass(m_postProcessPass, "PostProcessPass");
                m_frameGraph.addUserPass(m_imguiPass, "UIPass");

                m_frameGraph.setup(_dt);
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