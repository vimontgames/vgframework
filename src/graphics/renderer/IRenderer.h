#pragma once

#include "core/IPlugin.h"
#include "Renderer_consts.h"

namespace vg
{
    namespace core
    {
        class IProfiler;
        struct Singletons;
    }

    namespace graphics
    {
        namespace driver
        {
            class ITexture;
        }

        namespace renderer
        {
            class IImmediateGUI;
            class IMeshModel;
            class IView;

            struct CreateViewParams;

            class IRenderer : public core::IPlugin
            {
            public:
                IRenderer(const core::string & _name, core::IObject * _parent) :
                    core::IPlugin(_name, _parent)
                {

                }

                virtual void	                    init                (const RendererParams & _params, core::Singletons & _singletons) = 0;
                virtual void	                    deinit              () = 0;

                virtual IView *                     CreateMainView      (core::uint2 _screenSize) = 0;
                virtual IView *                     CreateView          (const CreateViewParams & _params) = 0;
                virtual const core::vector<IView *> GetViews            () const = 0;
                virtual void                        ReleaseView         (IView *& _view) = 0;

                virtual void                        SetResized          () = 0;
                virtual void                        resize              (core::uint _width, core::uint _height) = 0;
                virtual core::uint2                 getBackbufferSize   () const = 0;
                virtual void	                    runOneFrame         (double _dt) = 0;
                virtual void                        updateShaders       () = 0;
                virtual void                        waitGPUIdle         () = 0;

                virtual void                        SetVSync            (driver::VSync mode) = 0;
                virtual driver::VSync               GetVSync            () const = 0;

                #ifdef _WIN32
                virtual LRESULT CALLBACK            WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
                #endif

                virtual core::IProfiler *           getProfilerInstance () const = 0;
                virtual IImmediateGUI *             getImmediateGUI     () const = 0;

                virtual bool                        cookMeshModel       (const core::string & _file) = 0;
                virtual IMeshModel *                loadMeshModel       (const core::string & _file) = 0;

                virtual bool                        cookTexture         (const core::string & _file) = 0;
                virtual driver::ITexture *          loadTexture         (const core::string & _file) = 0;
            };
        }
    }
}