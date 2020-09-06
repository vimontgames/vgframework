#pragma once

#include "graphics/renderer/IRenderer.h"
#include "core/Singleton/Singleton.h"

namespace vg::graphics::driver
{
	class Device;
	class Texture;
	class Buffer;
	class FrameGraph;
}

namespace vg::graphics::renderer
{
    class TestPass;

	class Renderer : public IRenderer, public core::Singleton<Renderer>
	{
	public:
		IPlugin::Version		getVersion			() const override;

								Renderer			();
								~Renderer			();

		void					init				(const RendererParams & _params) override;
		void					deinit				() override;

		void					runOneFrame			() override;

		driver::Texture *		getBackbuffer		();

	private:
		driver::Device &		m_device;
		driver::FrameGraph &	m_frameGraph;
        TestPass *              m_testPass = nullptr;
	};
}