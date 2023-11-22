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
    class ImGuiAdapter;
    class FBXImporter;
    class ImGuiPass;
    class MeshModel;
    class MaterialModel;
    class View;
    class DebugDraw;

    enum class MaterialTextureType : core::u8;

	class Renderer : public IRenderer, public core::Singleton<Renderer>
	{
	public:
        using super = IRenderer;

		IPlugin::Version		                GetVersion			    () const override;

								                Renderer			    (const core::string & _name, core::IObject * _parent);
								                ~Renderer			    ();

        const char *                            getClassName            () const final { return "Renderer"; }
        bool                                    RegisterClasses         () override;
        bool                                    UnregisterClasses       ();
        static bool                             registerProperties      (core::IClassDesc & _desc);
 
		void					                init				    (const RendererCreationParams & _params, core::Singletons & _singletons) override;
		void					                deinit				    () override;

        const gfx::DeviceParams &               GetDeviceCreationParams () const final override;
        core::IObject *                         GetDisplayOptions       () final override;

        gfx::ITexture *                         CreateTexture           (const gfx::TextureDesc & _texDesc, const core::string & _name) final override;

        gfx::IView *                            CreateView              (gfx::CreateViewParams _params, const core::string & _name, gfx::IView::Flags _flags) final override;
        gfx::ViewID                             AddView                 (gfx::IView * _view) final override;
        void                                    RemoveView              (gfx::ViewID _viewID) final override;
        gfx::IView *                            GetView                 (gfx::ViewID _viewID) const final override;
        const core::vector <gfx::IView *>       GetViews                (gfx::ViewTarget _target) const final override;

        void                                    SetResized              () final override;
        void                                    resize                  (core::uint _width, core::uint _height) override;
        core::uint2                             getBackbufferSize       () const override;

		void					                runOneFrame			    (double _dt) override;

        void                                    updateShaders           () override;
        void                                    waitGPUIdle             () override;

        void                                    SetVSync                (gfx::VSync mode) final override;
        gfx::VSync                              GetVSync                () const final override;

        core::IProfiler *                       GetProfiler             () const final override;
        IImGuiAdapter *                         GetImGuiAdapter         () const final override;
        gfx::IShaderManager *                   GetShaderManager        () const final override;
        IPicking *                              GetPicking              () const final override;
        IDebugDraw *                            GetDebugDraw            () const final override;

        bool                                    cookMeshModel           (const core::string & _file) final override;
        IMeshModel *                            loadMeshModel           (const core::string & _file) final override;

        bool                                    cookTexture             (const core::string & _file) final override;
        gfx::ITexture *                         loadTexture             (const core::string & _file) final override;

        bool                                    cookAnimation           (const core::string & _file) final override;
        IAnimation *                            loadAnimation           (const core::string & _file) final override;

        void                                    ReleaseAsync            (core::IObject * _object) final override;

        VG_INLINE gfx::Texture *                getDefaultTexture       (MaterialTextureType _type) const;
        VG_INLINE MaterialModel *               getDefaultMaterial      () const;
        DebugDraw *                             getDebugDraw            () const;
        
        #ifdef _WIN32
        LRESULT CALLBACK                        WndProc                 (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
        #endif     

        void                                    SetFullscreen           (bool _fullscreen) final override;
        bool                                    IsFullscreen            () const final override;

    public: // internal
        View *                                  getMainView             () const;

        gfx::Texture *		                    getBackbuffer           () const;
        ImGuiAdapter *                          getImGuiAdapter         () const { return m_imgui; }

    private:
        void                                    registerShaders         ();

        void                                    initDefaultTextures     ();
        void                                    deinitDefaultTextures   ();

        void                                    initDefaultMaterials    ();
        void                                    deinitDefaultMaterials  ();

        void                                    cullViews               ();

	private:
		gfx::Device &		                    m_device;
        gfx::FrameGraph &                       m_frameGraph;
        ImGuiAdapter *                          m_imgui                 = nullptr;
        FBXImporter *                           m_fbxImporter           = nullptr;
        View *                                  m_mainView              = nullptr;
        ImGuiPass *                             m_imguiPass             = nullptr; 
        core::vector<View *>                    m_views[core::enumCount<gfx::ViewTarget>()];
        IPicking *                              m_picking               = nullptr;
        core::vector<gfx::Texture *>            m_defaultTextures;
        MaterialModel *                         m_defaultMaterial       = nullptr;
        bool                                    m_fullscreen            = false;
	};
}

#if VG_ENABLE_INLINE
#include "Renderer.inl"
#endif