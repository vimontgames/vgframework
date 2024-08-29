#pragma once

#include "renderer/IRenderer.h"
#include "core/Singleton/Singleton.h"
#include "core/IScheduler.h"

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
    class GPUDebugUpdatePass;
    class InstanceDataUpdatePass;
    class ComputeSkinningPass;
    class BLASUpdatePass;
    class ImGuiPass;
    class MeshModel;
    class MaterialModel;
    class View;
    class Viewport;
    class DebugDraw;

    struct SharedCullingJobOutput;

    enum class MaterialTextureType : core::u8;

	class Renderer : public IRenderer, public core::Singleton<Renderer>
	{
	public:
        using super = IRenderer;

		IPlugin::Version		                GetVersion			        () const override;

								                Renderer			        (const core::string & _name, core::IObject * _parent);
								                ~Renderer			        ();

        const char *                            GetClassName                () const final { return "Renderer"; }
        bool                                    RegisterClasses             () override;
        bool                                    UnregisterClasses           ();
        static bool                             registerProperties          (core::IClassDesc & _desc);
 
		void					                init				        (const RendererCreationParams & _params, core::Singletons & _singletons) override;
		void					                deinit				        () override;

        void					                RunOneFrame			        () override;

        const gfx::DeviceParams &               GetDeviceCreationParams     () const final override;
        renderer::IRendererOptions *            GetOptions                  () final override;

        gfx::ITexture *                         CreateTexture               (const gfx::TextureDesc & _texDesc, const core::string & _name) final override;

        gfx::IViewport *                        CreateViewport              (const gfx::CreateViewportParams & _params, const core::string & _name, gfx::ViewportFlags _flags = (gfx::ViewportFlags)0x0) final override;
        gfx::ViewportID                         AddViewport                 (gfx::IViewport * _viewport) final override;
        gfx::ViewportIndex                      GetFreeViewportIndex        (gfx::ViewportTarget _target) final override;
        gfx::IViewport *                        GetViewport                 (gfx::ViewportID _viewportID) final override;
        const core::vector<gfx::IViewport *> &  GetViewports                (gfx::ViewportTarget _target) const final override;

        // TODO: remove
        gfx::IView *                            CreateView                  (gfx::CreateViewParams _params, const core::string & _name, gfx::IView::Flags _flags = (gfx::IView::Flags)0) final override;
        gfx::ViewID                             AddView                     (gfx::IView * _view) final override;
        void                                    RemoveView                  (gfx::ViewID _viewID) final override;
        gfx::ViewIndex                          GetFreeViewIndex            (gfx::ViewTarget _target) const final override;
        gfx::IView *                            GetView                     (gfx::ViewID _viewID) const final override;
        gfx::IView *                            GetView                     (gfx::ViewTarget _target, const core::IWorld * _world) const final override;
        const core::vector <gfx::IView *>       GetViews                    (gfx::ViewTarget _target) const final override;

        void                                    SetResized                  () final override;
        void                                    Resize                      (core::uint _width, core::uint _height) override;
        core::uint2                             GetBackbufferSize           () const override;
        void                                    UpdateShaders               () override;
        void                                    WaitGPUIdle                 () override;

        void                                    SetVSync                    (gfx::VSync mode) final override;
        gfx::VSync                              GetVSync                    () const final override;

        core::IProfiler *                       GetProfiler                 () const final override;
        IImGuiAdapter *                         GetImGuiAdapter             () const final override;
        gfx::IShaderManager *                   GetShaderManager            () const final override;
        IPicking *                              GetPicking                  () const final override;
        IDebugDraw *                            GetDebugDraw                () const final override;

        bool                                    cookMeshModel               (const core::string & _file) final override;
        IMeshModel *                            loadMeshModel               (const core::string & _file) final override;

        bool                                    cookTexture                 (const core::string & _file, const TextureImportSettings * _importSettings) final override;
        gfx::ITexture *                         loadTexture                 (const core::string & _file) final override;

        bool                                    cookAnimation               (const core::string & _file) final override;
        IAnimation *                            loadAnimation               (const core::string & _file) final override;

        void                                    ReleaseAsync                (core::IObject * _object) final override;

        VG_INLINE gfx::Texture *                getDefaultTexture           (MaterialTextureType _type) const;
        VG_INLINE MaterialModel *               getDefaultMaterial          () const;
        DebugDraw *                             getDebugDraw                () const;
        
        #ifdef _WIN32
        LRESULT CALLBACK                        WndProc                     (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
        #endif     

        void                                    SetFullscreen               (bool _fullscreen) final override;
        bool                                    IsFullscreen                () const final override;

        ILightInstance *                        CreateLightInstance         (const ILightDesc * _lightDesc) final override;

    public: // internal
        gfx::Texture *                          getBackbuffer               () const;
        ImGuiAdapter *                          getImGuiAdapter             () const { return m_imgui; }
        SharedCullingJobOutput *                getSharedCullingJobOutput   () const { return m_sharedCullingJobOutput; }

        core::JobSync *                         GetJobSync                  (JobSync _jobSync) final override { return &m_jobSync[core::asInteger(_jobSync)]; }

    private:
        void                                    registerShaders             ();

        void                                    initDefaultTextures         ();
        void                                    deinitDefaultTextures       ();

        void                                    initDefaultMaterials        ();
        void                                    deinitDefaultMaterials      ();

        void                                    cullViews                   ();

	//private:
    public:
		gfx::Device &		                    m_device;
        gfx::FrameGraph &                       m_frameGraph;
        ImGuiAdapter *                          m_imgui                     = nullptr;
        FBXImporter *                           m_fbxImporter               = nullptr;
        GPUDebugUpdatePass *                    m_gpuDebugUpdatePass        = nullptr;
        InstanceDataUpdatePass *                m_instanceDataUpdatePass    = nullptr;
        ComputeSkinningPass *                   m_computeSkinningPass       = nullptr;
        BLASUpdatePass *                        m_BLASUpdatePass            = nullptr;
        ImGuiPass *                             m_imguiPass                 = nullptr; 
        IPicking *                              m_picking                   = nullptr;
        core::vector<gfx::Texture *>            m_defaultTextures;
        MaterialModel *                         m_defaultMaterial           = nullptr;
        bool                                    m_fullscreen                = false;
        SharedCullingJobOutput *                m_sharedCullingJobOutput    = nullptr;

        core::vector<Viewport *>                m_viewports[core::enumCount<gfx::ViewportTarget>()];
        core::JobSync                           m_jobSync[core::enumCount<JobSync>()];

        // TODO : remove
        core::vector<View *>                    m_views[core::enumCount<gfx::ViewTarget>()];
	};
}

#if VG_ENABLE_INLINE
#include "Renderer.inl"
#endif