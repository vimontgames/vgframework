#pragma once

#include "renderer/IRenderer.h"
#include "core/Singleton/Singleton.h"

namespace vg::gfx
{
	class Device;
	class Texture;
	class Buffer;
	class FrameGraph;
}

namespace vg::renderer
{
    class ImguiAdapter;
    class FBXImporter;
    class ImguiPass;
    class MeshModel;
    class View;

    enum class MaterialTextureType : core::u8;

	class Renderer : public IRenderer, public core::Singleton<Renderer>
	{
	public:
        using super = IRenderer;

		IPlugin::Version		                getVersion			    () const override;

								                Renderer			    (const core::string & _name, core::IObject * _parent);
								                ~Renderer			    ();

        const char *                            getClassName            () const final { return "Renderer"; }
        bool                                    registerClasses         () override;
        bool                                    unregisterClasses       ();
        static bool                             registerProperties      (core::IClassDesc & _desc);
 
		void					                init				    (const RendererParams & _params, core::Singletons & _singletons) override;
		void					                deinit				    () override;

        gfx::IView *                         CreateMainView          (core::uint2 _screenSize) final override;
        gfx::ViewID                          AddView                 (gfx::IView * _view) final override;
        void                                    RemoveView              (gfx::ViewID _viewID) final override;
        gfx::IView *                         GetView                 (gfx::ViewID _viewID) const final override;
        const core::vector <gfx::IView *>    GetViews                (gfx::ViewType _viewType) const final override;

        void                                    SetResized              () final override;
        void                                    resize                  (core::uint _width, core::uint _height) override;
        core::uint2                             getBackbufferSize       () const override;

		void					                runOneFrame			    (double _dt) override;

        void                                    updateShaders           () override;
        void                                    waitGPUIdle             () override;

        void                                    SetVSync                (gfx::VSync mode) final override;
        gfx::VSync                           GetVSync                () const final override;

        core::IProfiler *                       getProfilerInstance     () const override;
        IImmediateGUI *                         getImmediateGUI         () const override;

        bool                                    cookMeshModel           (const core::string & _file) final;
        IMeshModel *                            loadMeshModel           (const core::string & _file) final;

        bool                                    cookTexture             (const core::string & _file) final;
        gfx::ITexture *                      loadTexture             (const core::string & _file) final;

        void                                    ReleaseAsync            (core::IObject * _object) final override;

        gfx::Texture *                       getDefaultTexture       (MaterialTextureType _type) const;
        
        #ifdef _WIN32
        LRESULT CALLBACK                        WndProc                 (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
        #endif     

    public: // internal
        View *                                  getMainView             () const;

        gfx::Texture *		                getBackbuffer           () const;
        ImguiAdapter *                          getImGuiAdapter         () const { return m_imgui; }

    private:
        void                                    registerShaders         ();

        void                                    initDefaultTextures     ();
        void                                    deinitDefaultTextures   ();

        void                                    cullViews               ();

	private:
		gfx::Device &		                m_device;
        ImguiAdapter *                          m_imgui                 = nullptr;
        FBXImporter *                           m_fbxImporter           = nullptr;
		gfx::FrameGraph &	                m_frameGraph;
        View *                                  m_mainView              = nullptr;
        core::vector<View *>                    m_views[core::enumCount<gfx::ViewType>()];
        ImguiPass *                             m_imguiPass             = nullptr;
        core::vector<gfx::Texture*>          m_defaultTextures;       
	};
}