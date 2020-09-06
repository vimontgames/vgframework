#include "graphics/renderer/Precomp.h"
#include "renderer.h"

#include "graphics/driver/device/device.h"
#include "graphics/driver/FrameGraph/FrameGraph.h"

#include "Pass/TestPass.h"

using namespace vg::core;
using namespace vg::graphics::driver;
using namespace vg::graphics::renderer;

//--------------------------------------------------------------------------------------
IRenderer * Create()
{
	return new Renderer();
}

//--------------------------------------------------------------------------------------
namespace vg::graphics::renderer
{
	#define VG_RENDERER_VERSION_MAJOR 0
	#define VG_RENDERER_VERSION_MINOR 1

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

	}

	//--------------------------------------------------------------------------------------
	Renderer::~Renderer()
	{
        m_frameGraph.release();
		m_device.release();
	}

	//--------------------------------------------------------------------------------------
	void Renderer::init(const RendererParams & _params)
	{
		m_device.init(_params.device);

        m_testPass = new TestPass();
	}

	//--------------------------------------------------------------------------------------
	void Renderer::deinit()
	{
        VG_SAFE_DELETE(m_testPass);

		m_device.deinit();
	}

	//--------------------------------------------------------------------------------------
	void Renderer::runOneFrame()
	{
		m_device.beginFrame();
		{
			m_frameGraph.import("Backbuffer", m_device.getBackbuffer());
			m_frameGraph.setGraphOutput("Backbuffer");
            
			m_frameGraph.addUserPass(m_testPass, "test");
            
			m_frameGraph.setup();
			m_frameGraph.build();
			m_frameGraph.render();
		}
		m_device.endFrame();
	}

	//--------------------------------------------------------------------------------------
	Texture * Renderer::getBackbuffer()
	{
		return m_device.getBackbuffer();
	}
}