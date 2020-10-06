#pragma once

#include "core/IPlugin.h"
#include "Renderer_consts.h"

namespace vg::graphics::renderer
{
	class IRenderer : public core::IPlugin
	{
	public:
		virtual void	            init			(const RendererParams & _params) = 0;
		virtual void	            deinit			() = 0;

		virtual void	            runOneFrame		() = 0;

        #ifdef _WIN32
        virtual LRESULT CALLBACK    WndProc         (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
        #endif
	};
}