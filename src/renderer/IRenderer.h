#pragma once

#include "core/IPlugin.h"
#include "Renderer_consts.h"
#include "gfx/IView.h"      
#include "gfx/IViewport.h"

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
        class IViewport;
        class IView;
        class IShaderManager;

        struct CreateViewParams;
        struct CreateViewportParams;
        struct TextureImporterSettings;

        enum class ViewportFlags : core::u32;
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
        
        struct RendererCreationParams;

        vg_enum_class(RendererJobType, core::u8,
            Culling,
            DebugDraw
        );

        class IRenderer : public core::IPlugin
        {
        public:
            IRenderer(const core::string & _name, core::IObject * _parent) :
                core::IPlugin(_name, _parent)
            {

            }

            virtual void	                            init                    (const RendererCreationParams & _params, core::Singletons & _singletons) = 0;
            virtual void	                            deinit                  () = 0;

            virtual void	                            RunOneFrame             () = 0;

            virtual const gfx::DeviceParams &           GetDeviceCreationParams () const = 0;
            virtual renderer::IRendererOptions *        GetOptions              () = 0;

            virtual gfx::ITexture *                     CreateTexture           (const gfx::TextureDesc & _texDesc, const core::string & _name) = 0;

            virtual gfx::IViewport *                    CreateViewport          (const gfx::CreateViewportParams & _params, const core::string & _name, gfx::ViewportFlags _flags = (gfx::ViewportFlags)0x0) = 0;
            virtual gfx::ViewportID                     AddViewport             (gfx::IViewport * _viewport) = 0;
            virtual gfx::ViewportIndex                  GetFreeViewportIndex    (gfx::ViewportTarget _target) = 0;
            virtual gfx::IViewport *                    GetViewport             (gfx::ViewportID _viewportID) = 0;
            virtual const core::vector<gfx::IViewport*>&GetViewports            (gfx::ViewportTarget _target) const = 0;

            // TODO: remove or move to IViewport (?)
            virtual gfx::IView *                        CreateView              (gfx::CreateViewParams _params, const core::string & _name, gfx::ViewFlags _flags = (gfx::ViewFlags)0) = 0;
            virtual gfx::ViewID                         AddView                 (gfx::IView * _view) = 0;
            virtual void                                RemoveView              (gfx::ViewID _viewID) = 0;
            virtual gfx::ViewIndex                      GetFreeViewIndex        (gfx::ViewTarget _target) const = 0;
            virtual gfx::IView *                        GetView                 (gfx::ViewID _viewID) const = 0;
            virtual gfx::IView *                        GetView                 (gfx::ViewTarget _target, const core::IWorld * _world) const = 0;
            virtual const core::vector<gfx::IView *>    GetViews                (gfx::ViewTarget _target) const = 0;

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