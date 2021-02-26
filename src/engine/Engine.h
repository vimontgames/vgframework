#pragma once

#include "engine/IEngine.h"

namespace vg::engine
{
	class Engine : public IEngine
	{
	public:
		IPlugin::Version				getVersion	() const override;

										Engine		();
										~Engine		();

        #ifdef _WIN32
        LRESULT CALLBACK                WndProc     (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
        #endif

		void							init		(const EngineParams & _params, core::Singletons & _singletons) override;
		void							deinit		() override;

		void							runOneFrame	() override;

		graphics::renderer::IRenderer *	getRenderer	() const override;

    protected:
        void                            updateDt    ();

	private:
		graphics::renderer::IRenderer *	m_renderer = nullptr;
        double                          m_dt = 0.0f;
	};
}