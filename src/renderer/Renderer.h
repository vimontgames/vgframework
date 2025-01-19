#pragma once

#include "renderer/IRenderer.h"
#include "core/Singleton/Singleton.h"
#include "core/IScheduler.h"

namespace vg::gfx
{
	class Device;
    class DeviceCaps;
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
    class MaterialDataUpdatePass;
    class ComputeSkinningPass;
    class BLASUpdatePass;
    class ComputeSpecularBRDFPass;
    class ComputeIBLCubemapsPass;
    class ImGuiPass;
    class HDROutputPass;
    class MeshModel;
    class MaterialModel;
    class View;
    class Viewport;
    class DebugDraw;
    class MaterialManager;

    struct SharedCullingJobOutput;

    enum class MaterialTextureType : core::u8;

	class Renderer final : public IRenderer, public core::Singleton<Renderer>
	{
	public:
        using super = IRenderer;

								                Renderer			        (const core::string & _name, core::IObject * _parent);
								                ~Renderer			        ();

        const char *                            GetClassName                () const final override { return "Renderer"; }
        bool                                    RegisterClasses             () final override;
        bool                                    UnregisterClasses           ();
        static bool                             registerProperties          (core::IClassDesc & _desc);
 
		void					                Init				        (const RendererCreationParams & _params, core::Singletons & _singletons) final override;
		void					                Deinit				        () final override;

        void	                                CreateResources             () final override;
        void	                                ReleaseResources            () final override;

        void					                RunOneFrame			        () final override;

        const gfx::DeviceParams &               GetDeviceCreationParams     () const final override;
        const renderer::IRendererOptions *      GetOptions                  () const final override;
        renderer::IRendererOptions *            GetOptions                  () final override;

        gfx::ITexture *                         CreateTexture               (const gfx::TextureDesc & _texDesc, const core::string & _name) final override;

        IViewport *                             CreateViewport              (const CreateViewportParams & _params, const core::string & _name, ViewportFlags _flags = (ViewportFlags)0x0) final override;
        gfx::ViewportID                         AddViewport                 (IViewport * _viewport) final override;
        gfx::ViewportIndex                      GetFreeViewportIndex        (gfx::ViewportTarget _target) final override;
        IViewport *                             GetViewport                 (gfx::ViewportID _viewportID) final override;
        const core::vector<IViewport *> &       GetViewports                (gfx::ViewportTarget _target) const final override;

        // TODO: remove
        IView *                                 CreateView                  (CreateViewParams _params, const core::string & _name, ViewFlags _flags = (ViewFlags)0) final override;
        gfx::ViewID                             AddView                     (IView * _view) final override;
        void                                    RemoveView                  (gfx::ViewID _viewID) final override;
        gfx::ViewIndex                          GetFreeViewIndex            (gfx::ViewTarget _target) const final override;
        IView *                                 GetView                     (gfx::ViewID _viewID) const final override;
        IView *                                 GetView                     (gfx::ViewTarget _target, const core::IWorld * _world) const final override;
        const core::vector <IView *>            GetViews                    (gfx::ViewTarget _target) const final override;

        const ICameraLens *                     GetDefaultCameraLens        () const final override;

        void                                    SetResized                  () final override;
        void                                    Resize                      (core::uint _width, core::uint _height) final override;
        void                                    Move                        () final override;
        core::uint2                             GetBackbufferSize           () const override;
        void                                    UpdateShaders               () override;
        void                                    WaitGPUIdle                 () override;

        void                                    SetVSync                    (gfx::VSync _mode) final override;
        gfx::VSync                              GetVSync                    () const final override;

        void                                    SetHDR                      (gfx::HDR _mode) final override;
        gfx::HDR                                GetHDR                      () const final override;

        core::IProfiler *                       GetProfiler                 () const final override;
        IImGuiAdapter *                         GetImGuiAdapter             () const final override;
        gfx::IShaderManager *                   GetShaderManager            () const final override;
        IPicking *                              GetPicking                  () const final override;
        IDebugDraw *                            GetDebugDraw                () const final override;
        IUIManager *                            GetUIManager                () const final override;

        bool                                    CookMeshModel               (const core::string & _file, const gfx::MeshImporterSettings * _importSettings) final override;
        IMeshModel *                            LoadMeshModel               (const core::string & _file) final override;

        bool                                    CookTexture                 (const core::string & _file, const gfx::TextureImporterSettings * _importSettings) final override;
        gfx::ITexture *                         LoadTexture                 (const core::string & _file) final override;

        bool                                    CookAnimation               (const core::string & _file) final override;
        IAnimation *                            LoadAnimation               (const core::string & _file) final override;

        void                                    ReleaseAsync                (core::IObject * _object) final override;

        VG_INLINE gfx::Texture *                getDefaultTexture           (MaterialTextureType _type) const;
        VG_INLINE MaterialModel *               getDefaultMaterial          () const;
        DebugDraw *                             getDebugDraw                () const;
        
        #ifdef _WIN32
        LRESULT CALLBACK                        WndProc                     (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
        #endif     

        void                                    SetFullscreen               (bool _fullscreen) final override;
        bool                                    IsFullscreen                () const final override;
        bool                                    IsEditor                    () const final override;

        ILightInstance *                        CreateLightInstance         (const ILightDesc * _lightDesc) final override;

        bool                                    IsVSyncSupported            (gfx::VSync _mode) const final override;
        bool                                    IsHDRSupported              (gfx::HDR _mode) const final override;
        bool                                    IsMSAASupported             (gfx::MSAA _mode) const final override;

        core::JobSync *                         GetJobSync                  (RendererJobType _jobSync) final override;
        bool                                    WaitJobSync                 (RendererJobType _jobSync) final override;

        double                                  GetGpuFrameTime             () const final override;
        double                                  GetGpuWaitTime              () const final override;

        const gfx::DeviceCaps &                 getDeviceCaps               () const;

        gfx::Texture *                          getSpecularBRDF             () const;

    public: // internal
        gfx::Texture *                          getBackbuffer               () const;

        VG_INLINE ImGuiAdapter *                getImGuiAdapter             () const;
        VG_INLINE SharedCullingJobOutput *      getSharedCullingJobOutput   () const;
        VG_INLINE MaterialManager *             getMaterialManager          () const;

    private:
        void                                    registerShaders             ();
        static void                             onShadersUpdated            (bool _success);

        void                                    initDefaultTextures         ();
        void                                    deinitDefaultTextures       ();

        void                                    initDefaultMaterials        ();
        void                                    deinitDefaultMaterials      ();

        void                                    cullViews                   ();

        void                                    releaseSpecularBRDF         ();

	private:
        bool                                    m_editor                    = false;
		gfx::Device &		                    m_device;
        gfx::FrameGraph &                       m_frameGraph;
        gfx::Texture *                          m_hdrOutput                 = nullptr;
        ImGuiAdapter *                          m_imgui                     = nullptr;
        FBXImporter *                           m_fbxImporter               = nullptr;
        GPUDebugUpdatePass *                    m_gpuDebugUpdatePass        = nullptr;
        InstanceDataUpdatePass *                m_instanceDataUpdatePass    = nullptr;
        MaterialDataUpdatePass *                m_materialDataUpdatePass    = nullptr;
        ComputeSkinningPass *                   m_computeSkinningPass       = nullptr;
        BLASUpdatePass *                        m_BLASUpdatePass            = nullptr;
        ComputeSpecularBRDFPass *               m_computeSpecularBRDFPass   = nullptr;
        ComputeIBLCubemapsPass *                m_computeIBLCubemapsPass    = nullptr;
        ImGuiPass *                             m_imguiPass                 = nullptr; 
        HDROutputPass *                         m_hdrOutputPass             = nullptr;
        IPicking *                              m_picking                   = nullptr;
        core::vector<gfx::Texture *>            m_defaultTextures;
        MaterialModel *                         m_defaultMaterial           = nullptr;
        bool                                    m_fullscreen                = false;
        SharedCullingJobOutput *                m_sharedCullingJobOutput    = nullptr;

        core::vector<Viewport *>                m_viewports[core::enumCount<gfx::ViewportTarget>()];

        struct JobSyncInfo
        {
            core::atomic<bool> started = false;
            core::JobSync id;
        };
        JobSyncInfo                             m_jobSyncInfo[core::enumCount<RendererJobType>()];

        // TODO : remove
        core::vector<renderer::View *>          m_views[core::enumCount<gfx::ViewTarget>()];

        gfx::Texture *                          m_generatedSpecularBRDF     = nullptr;
        MaterialManager *                       m_materialManager           = nullptr;
        ICameraLens *                           m_defaultCameraLens         = nullptr;
	};
}

#if VG_ENABLE_INLINE
#include "Renderer.inl"
#endif