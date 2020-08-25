#include "graphics/renderer/Precomp.h"
#include "renderer.h"

#include "graphics/driver/device/device.h"
#include "graphics/driver/CommandList/CommandList.h"
#include "graphics/driver/FrameGraph/FrameGraph.h"
#include "graphics/driver/FrameGraph/UserPass.h"
#include "graphics/driver/Texture/Texture.h"

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
		m_device.release();
		m_frameGraph.release();
	}

	//--------------------------------------------------------------------------------------
	void Renderer::init(const RendererParams & _params)
	{
		m_device.init(_params.device);
	}

	//--------------------------------------------------------------------------------------
	void Renderer::deinit()
	{
		m_device.deinit();
	}

	class TestPass : public driver::UserPass
	{
	public:
		void setup() override
		{
			static uint frame = 0;
			const auto clearColor = float4(frame & 1, frame & 2, frame & 4, 0);
			++frame;

			auto * renderer = Renderer::get();
			auto & backbuffer = renderer->getBackbuffer()->getTexDesc();

			FrameGraph::TextureDesc desc;
									desc.width = backbuffer.width;
									desc.height = backbuffer.height;
									desc.format = PixelFormat::R8G8B8A8;
									desc.initState = FrameGraph::Resource::InitState::Clear;
									desc.clearColor = clearColor;

			writeRenderTarget(0, "Backbuffer", desc);
		}

		void draw(driver::CommandList * _cmdList) const override
		{

		}
	};

	//--------------------------------------------------------------------------------------
	void Renderer::runOneFrame()
	{
		m_device.beginFrame();
		{
			m_frameGraph.reset();

			m_frameGraph.import("Backbuffer", m_device.getBackbuffer());
			m_frameGraph.setGraphOutput("Backbuffer");

			auto & test = m_frameGraph.addUserPass<TestPass>("testPass");

			m_frameGraph.setup();
			m_frameGraph.build();
			m_frameGraph.render();

		}
		m_device.endFrame();

		Sleep(1000);
	}

	//--------------------------------------------------------------------------------------
	Texture * Renderer::getBackbuffer()
	{
		return m_device.getBackbuffer();
	}
}