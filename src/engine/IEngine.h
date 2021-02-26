#pragma once

#include "core/IPlugin.h"
#include "Engine_Consts.h"

namespace vg::core
{
    struct Singletons;
}

namespace vg::graphics::renderer
{
	class IRenderer;
}

namespace vg::engine
{
	class IEngine : public core::IPlugin
	{
	public:
		virtual void							init		(const EngineParams & _params, core::Singletons & _singletons) = 0;
		virtual void							deinit		() = 0;

        #ifdef _WIN32
        virtual LRESULT CALLBACK                WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
        #endif
        
		virtual void							runOneFrame	() = 0;

		virtual graphics::renderer::IRenderer *	getRenderer	() const = 0;
	};
}