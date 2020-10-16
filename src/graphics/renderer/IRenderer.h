#pragma once

#include "core/IPlugin.h"
#include "Renderer_consts.h"

namespace vg
{
    namespace core
    {
        class IProfiler;
    }

    namespace graphics::renderer
    {
        class IRenderer : public core::IPlugin
        {
        public:
            virtual void	            init(const RendererParams & _params) = 0;
            virtual void	            deinit() = 0;
            virtual void                resize(core::uint _width, core::uint _height) = 0;
            virtual void	            runOneFrame() = 0;
            virtual void                updateShaders() = 0;

            #ifdef _WIN32
            virtual LRESULT CALLBACK    WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
            #endif

            virtual core::IProfiler *   getProfilerInstance() const = 0;
        };
    }
}