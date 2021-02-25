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

    class BackgroundPass;
    class TestPass;
    class PostProcessPass;
    class ImguiPass;

	class Renderer : public IRenderer, public core::Singleton<Renderer>
	{
	public:
		IPlugin::Version		getVersion			() const override;

								Renderer			();
								~Renderer			();

		void					init				(const RendererParams & _params) override;
		void					deinit				() override;
        void                    resize              (core::uint _width, core::uint _height) override;
		void					runOneFrame			(double _dt) override;
        void                    updateShaders       () override;

		driver::Texture *		getBackbuffer		();
        core::IProfiler *       getProfilerInstance () const override;

        ImguiAdapter *          getImGuiAdapter     () const { return m_imgui; }
        
        #ifdef _WIN32
        LRESULT CALLBACK        WndProc             (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
        #endif        

    private:
        void                    registerShaders     ();

	private:
		driver::Device &		m_device;
        ImguiAdapter *          m_imgui = nullptr;
		driver::FrameGraph &	m_frameGraph;

        BackgroundPass *        m_backgroundPass    = nullptr;
        TestPass *              m_testPass          = nullptr;
        PostProcessPass *       m_postProcessPass   = nullptr;
        ImguiPass *             m_imguiPass         = nullptr;
	};
}