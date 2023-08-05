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
    }

    namespace gfx
    {
        class ITexture;
        class IView;
    }

    namespace renderer
    {
        class IImmediateGUI;
        class IMeshModel;

        struct CreateViewParams;

        class IRenderer : public core::IPlugin
        {
        public:
            IRenderer(const core::string & _name, core::IObject * _parent) :
                core::IPlugin(_name, _parent)
            {

            }

            virtual void	                            init                (const RendererParams & _params, core::Singletons & _singletons) = 0;
            virtual void	                            deinit              () = 0;

            virtual gfx::IView *                        CreateMainView      (core::uint2 _screenSize) = 0;
            virtual gfx::ViewID                         AddView             (gfx::IView * _view) = 0;
            virtual void                                RemoveView          (gfx::ViewID _viewID) = 0;
            virtual gfx::IView *                        GetView             (gfx::ViewID _viewID) const = 0;
            virtual const core::vector<gfx::IView *>    GetViews            (gfx::ViewType _viewType) const = 0;

            virtual void                                SetResized          () = 0;
            virtual void                                resize              (core::uint _width, core::uint _height) = 0;
            virtual core::uint2                         getBackbufferSize   () const = 0;
            virtual void	                            runOneFrame         (double _dt) = 0;
            virtual void                                updateShaders       () = 0;
            virtual void                                waitGPUIdle         () = 0;

            virtual void                                SetVSync            (gfx::VSync mode) = 0;
            virtual gfx::VSync                          GetVSync            () const = 0;

            #ifdef _WIN32
            virtual LRESULT CALLBACK                    WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
            #endif

            virtual core::IProfiler *                   getProfilerInstance () const = 0;
            virtual IImmediateGUI *                     getImmediateGUI     () const = 0;

            virtual bool                                cookMeshModel       (const core::string & _file) = 0;
            virtual IMeshModel *                        loadMeshModel       (const core::string & _file) = 0;

            virtual bool                                cookTexture         (const core::string & _file) = 0;
            virtual gfx::ITexture *                     loadTexture         (const core::string & _file) = 0;

            virtual void                                ReleaseAsync        (core::IObject * _object) = 0;
        };
    }
}