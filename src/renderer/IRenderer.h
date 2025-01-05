#pragma once

#include "core/IPlugin.h"
#include "Renderer_consts.h"
#include "renderer/IView.h"      
#include "renderer/IViewport.h"

namespace vg
{
    namespace core
    {
        class IProfiler;
        struct Singletons;
        struct JobSync;
    }

    namespace gfx
    {
        class ITexture;
        class TextureDesc;
        class IShaderManager;
        struct TextureImporterSettings;

        enum class ViewportFlags : core::u32;
        enum class RenderJobsPolicy : core::u8;
    }

    namespace renderer
    {
        class IImGuiAdapter;
        class IMeshModel;
        class IPicking;
        class IMaterialData;
        class IAnimation;
        class IDebugDraw;
        class IRendererOptions;
        class ILightInstance;
        class ILightDesc;
        class IUIManager;
        class IViewport;
        class IView;
        class ICameraLens;
        
        struct RendererCreationParams;
        struct CreateViewParams;
        struct CreateViewportParams;

        vg_enum_class(RendererJobType, core::u8,
            Culling,
            Render,
            DebugDraw
        );

        class IRenderer : public core::IPlugin
        {
        public:
            IRenderer(const core::string & _name, core::IObject * _parent) :
                core::IPlugin(_name, _parent)
            {

            }

            virtual void	                            Init                    (const RendererCreationParams & _params, core::Singletons & _singletons) = 0;
            virtual void	                            Deinit                  () = 0;

            virtual void	                            ReleaseResources        () = 0;

            virtual void	                            RunOneFrame             () = 0;

            virtual const gfx::DeviceParams &           GetDeviceCreationParams () const = 0;
            virtual renderer::IRendererOptions *        GetOptions              () = 0;

            virtual gfx::ITexture *                     CreateTexture           (const gfx::TextureDesc & _texDesc, const core::string & _name) = 0;

            virtual IViewport *                         CreateViewport          (const CreateViewportParams & _params, const core::string & _name, ViewportFlags _flags = (ViewportFlags)0x0) = 0;
            virtual gfx::ViewportID                     AddViewport             (IViewport * _viewport) = 0;
            virtual gfx::ViewportIndex                  GetFreeViewportIndex    (gfx::ViewportTarget _target) = 0;
            virtual IViewport *                         GetViewport             (gfx::ViewportID _viewportID) = 0;
            virtual const core::vector<IViewport*>&     GetViewports            (gfx::ViewportTarget _target) const = 0;

            // TODO: remove or move to IViewport (?)
            virtual IView *                             CreateView              (CreateViewParams _params, const core::string & _name, ViewFlags _flags = (ViewFlags)0) = 0;
            virtual gfx::ViewID                         AddView                 (IView * _view) = 0;
            virtual void                                RemoveView              (gfx::ViewID _viewID) = 0;
            virtual gfx::ViewIndex                      GetFreeViewIndex        (gfx::ViewTarget _target) const = 0;
            virtual IView *                             GetView                 (gfx::ViewID _viewID) const = 0;
            virtual IView *                             GetView                 (gfx::ViewTarget _target, const core::IWorld * _world) const = 0;
            virtual const core::vector<IView *>         GetViews                (gfx::ViewTarget _target) const = 0;

            virtual const ICameraLens *                 GetDefaultCameraLens    () const = 0;

            virtual void                                SetResized              () = 0;
            virtual void                                Resize                  (core::uint _width, core::uint _height) = 0;
            virtual void                                Move                    () = 0;
            virtual core::uint2                         GetBackbufferSize       () const = 0;
            virtual void                                UpdateShaders           () = 0;
            virtual void                                WaitGPUIdle             () = 0;

            virtual void                                SetVSync                (gfx::VSync _mode) = 0;
            virtual gfx::VSync                          GetVSync                () const = 0;

            virtual void                                SetHDR                  (gfx::HDR _mode) = 0;
            virtual gfx::HDR                            GetHDR                  () const = 0;

            virtual bool                                SetMaxRenderJobCount    (core::uint _maxCount, gfx::RenderJobsPolicy _scheduling) = 0;
            virtual core::uint                          GetMaxRenderJobCount    () const = 0;

            #ifdef _WIN32
            virtual LRESULT CALLBACK                    WndProc                 (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
            #endif

            virtual core::IProfiler *                   GetProfiler             () const = 0;
            virtual IImGuiAdapter *                     GetImGuiAdapter         () const = 0;
            virtual gfx::IShaderManager *               GetShaderManager        () const = 0;
            virtual IPicking *                          GetPicking              () const = 0;
            virtual IDebugDraw *                        GetDebugDraw            () const = 0;
            virtual IUIManager *                        GetUIManager            () const = 0;

            virtual bool                                cookMeshModel           (const core::string & _file) = 0;
            virtual IMeshModel *                        loadMeshModel           (const core::string & _file) = 0;

            virtual bool                                cookTexture             (const core::string & _file, const gfx::TextureImporterSettings * _importSettings) = 0;
            virtual gfx::ITexture *                     loadTexture             (const core::string & _file) = 0;

            virtual bool                                cookAnimation           (const core::string & _file) = 0;
            virtual IAnimation *                        loadAnimation           (const core::string & _file) = 0;

            virtual void                                ReleaseAsync            (core::IObject * _object) = 0;

            virtual void                                SetFullscreen           (bool _fullscreen) = 0;
            virtual bool                                IsFullscreen            () const = 0;
            virtual bool                                IsEditor                () const = 0;

            virtual ILightInstance *                    CreateLightInstance     (const ILightDesc * _lightDesc) = 0;

            virtual bool                                IsHDRSupported          (gfx::HDR _mode) const = 0;
            virtual bool                                IsVSyncSupported        (gfx::VSync _mode) const = 0;
            virtual bool                                IsMSAASupported         (gfx::MSAA _mode) const = 0;

            virtual core::JobSync *                     GetJobSync              (RendererJobType _jobSync) = 0;

            virtual double                              GetGpuFrameTime         () const = 0;
            virtual double                              GetGpuWaitTime          () const = 0;
        };
    }
}