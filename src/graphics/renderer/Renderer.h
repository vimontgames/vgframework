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
    class ImguiAdapter;

    class TestPass;
    class ImguiPass;

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

        #ifdef _WIN32
        LRESULT CALLBACK        WndProc             (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
        #endif

	//private:
		driver::Device &		m_device;
        ImguiAdapter *                 m_imgui = nullptr;
		driver::FrameGraph &	m_frameGraph;

        TestPass *              m_testPass = nullptr;
        ImguiPass *             m_imguiPass = nullptr;
	};
}