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
    class FBXImporter;
    class View;
    class BackgroundPass;
    class TestPass3D;
    class TestPass2D;
    class PostProcessPass;
    class ImguiPass;
    class MeshModel;

	class Renderer : public IRenderer, public core::Singleton<Renderer>
	{
	public:
		IPlugin::Version		getVersion			() const override;

								Renderer			();
								~Renderer			();

        const char *            getClassName        () const final { return "Renderer"; }
        bool                    registerClasses     () override;
 
		void					init				(const RendererParams & _params, core::Singletons & _singletons) override;
		void					deinit				() override;

        IView *                 createView          (const CreateViewParams & _params) override;
        void                    setView             (IView * _view) override;

        void                    resize              (core::uint _width, core::uint _height) override;
        core::uint2             getBackbufferSize   () const override;

		void					runOneFrame			(double _dt) override;

        void                    updateShaders       () override;
        void                    waitGPUIdle         () override;

        core::IProfiler *       getProfilerInstance () const override;

        MeshModel *             createMeshModel     (const core::string & _path);
        
        #ifdef _WIN32
        LRESULT CALLBACK        WndProc             (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
        #endif     

    public: // internal
        View *                  getView             () const;

        driver::Texture *		getBackbuffer       () const;
        ImguiAdapter *          getImGuiAdapter     () const { return m_imgui; }

    private:
        void                    registerShaders     ();

	private:
		driver::Device &		m_device;
        ImguiAdapter *          m_imgui             = nullptr;
        FBXImporter *           m_fbxImporter       = nullptr;
		driver::FrameGraph &	m_frameGraph;
        View *                  m_view              = nullptr;
        BackgroundPass *        m_backgroundPass    = nullptr;
        TestPass3D *            m_testPass3D        = nullptr;
        TestPass2D *            m_testPass2D        = nullptr;
        PostProcessPass *       m_postProcessPass   = nullptr;
        ImguiPass *             m_imguiPass         = nullptr;
	};
}