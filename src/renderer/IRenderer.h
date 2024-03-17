#pragma once

#include "core/IPlugin.h"
#include "Renderer_consts.h"
#include "gfx/IView.h"

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
        class IView;
        class IShaderManager;
        struct CreateViewParams;
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

        struct RendererCreationParams;

        class IRenderer : public core::IPlugin
        {
        public:

            enum class JobSync
            {
                Culling,
                DebugDraw
            };

            IRenderer(const core::string & _name, core::IObject * _parent) :
                core::IPlugin(_name, _parent)
            {

            }

            virtual void	                            init                    (const RendererCreationParams & _params, core::Singletons & _singletons) = 0;
            virtual void	                            deinit                  () = 0;

            virtual const gfx::DeviceParams &           GetDeviceCreationParams () const = 0;
            virtual renderer::IRendererOptions *        GetOptions              () = 0;

            virtual gfx::ITexture *                     CreateTexture           (const gfx::TextureDesc & _texDesc, const core::string & _name) = 0;

            virtual gfx::IView *                        CreateView              (gfx::CreateViewParams _params, const core::string & _name, gfx::IView::Flags _flags = (gfx::IView::Flags)0) = 0;
            virtual gfx::ViewID                         AddView                 (gfx::IView * _view) = 0;
            virtual void                                RemoveView              (gfx::ViewID _viewID) = 0;
            virtual gfx::IView *                        GetView                 (gfx::ViewID _viewID) const = 0;
            virtual gfx::IView *                        GetView                 (gfx::ViewTarget _target, core::IWorld * _world) const = 0;
            virtual const core::vector<gfx::IView *>    GetViews                (gfx::ViewTarget _target) const = 0;

            virtual void                                SetResized              () = 0;
            virtual void                                resize                  (core::uint _width, core::uint _height) = 0;
            virtual core::uint2                         getBackbufferSize       () const = 0;
            virtual void	                            runOneFrame             () = 0;
            virtual void                                updateShaders           () = 0;
            virtual void                                waitGPUIdle             () = 0;

            virtual void                                SetVSync                (gfx::VSync mode) = 0;
            virtual gfx::VSync                          GetVSync                () const = 0;

            #ifdef _WIN32
            virtual LRESULT CALLBACK                    WndProc                 (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
            #endif

            virtual core::IProfiler *                   GetProfiler             () const = 0;
            virtual IImGuiAdapter *                     GetImGuiAdapter         () const = 0;
            virtual gfx::IShaderManager *               GetShaderManager        () const = 0;
            virtual IPicking *                          GetPicking              () const = 0;
            virtual IDebugDraw *                        GetDebugDraw            () const = 0;

            virtual bool                                cookMeshModel           (const core::string & _file) = 0;
            virtual IMeshModel *                        loadMeshModel           (const core::string & _file) = 0;

            virtual bool                                cookTexture             (const core::string & _file) = 0;
            virtual gfx::ITexture *                     loadTexture             (const core::string & _file) = 0;

            virtual bool                                cookAnimation           (const core::string & _file) = 0;
            virtual IAnimation *                        loadAnimation           (const core::string & _file) = 0;

            virtual void                                ReleaseAsync            (core::IObject * _object) = 0;

            virtual void                                SetFullscreen           (bool _fullscreen) = 0;
            virtual bool                                IsFullscreen            () const = 0;

            virtual ILightInstance *                    CreateLightInstance     (const ILightDesc * _lightDesc) = 0;

            virtual core::JobSync *                     GetJobSync              (JobSync _jobSync) = 0;
        };
    }
}