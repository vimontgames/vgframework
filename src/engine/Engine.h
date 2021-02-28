#pragma once

#include "engine/IEngine.h"

namespace vg::core
{
    class Entity;
}

namespace vg::graphics::renderer
{
    class IView;
}

namespace vg::engine
{
    class FreeCamEntity;

	class Engine : public IEngine
	{
	public:
		IPlugin::Version				getVersion	        () const override;

										Engine		        ();
										~Engine		        ();

        #ifdef _WIN32
        LRESULT CALLBACK                WndProc             (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
        #endif

		void							init		        (const EngineParams & _params, core::Singletons & _singletons) override;
		void							deinit		        () override;

		void							runOneFrame	        () override;

        core::uint2                     getScreenSize       () const override;
		graphics::renderer::IRenderer *	getRenderer	        () const override;

    public:
        void                            createEditorView    ();
        void                            destroyEditorView   ();

        void                            addEntity           (core::Entity * _entity);
        void                            destroyEntities     (); 

    protected:
        void                            updateDt            ();
        void                            updateEntities      (double _dt);

	private:
		graphics::renderer::IRenderer *	m_renderer      = nullptr;
        graphics::renderer::IView *     m_editorView    = nullptr;
        FreeCamEntity *                 m_freeCam       = nullptr;

        core::vector<core::Entity*>     m_entities;

        double                          m_dt = 0.0f;
	};
}