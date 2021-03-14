#pragma once

#include "engine/IEngine.h"
#include "core/Singleton/Singleton.h"
#include "core/Resource/Resource.h"

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

	class Engine : public IEngine, public core::Singleton<Engine>
	{
	public:
        using super = IEngine;

		IPlugin::Version				getVersion	        () const final;

										Engine		        ();
										~Engine		        ();

        const char *                    getClassName        () const final { return "Engine"; }
        bool                            registerClasses     () override;
        static bool                     registerProperties  (core::IObjectDescriptor & _desc);

        static bool                     createProject       (core::IObject * _engine);
        static bool                     loadProject         (core::IObject * _engine);
        static bool                     saveProject         (core::IObject * _engine);

        #ifdef _WIN32
        LRESULT CALLBACK                WndProc             (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) final;
        #endif

		void							init		        (const EngineParams & _params, core::Singletons & _singletons) final;
		void							deinit		        () override;

        bool                            loadProject         (const core::string & _name) final;
        bool                            unloadProject       () final;
        IProject *                      getProject          () const final;

		void							runOneFrame	        () final;

        core::uint2                     getScreenSize       () const final;
		graphics::renderer::IRenderer *	getRenderer	        () const final;

    public:
        void                            createEditorView    ();
        void                            destroyEditorView   ();

        void                            addEntity           (core::Entity * _entity);
        void                            destroyEntities     (); 

    protected:
        void                            updateDt            ();
        void                            updateEntities      (double _dt);

	private:
        core::Resource                  m_project;

		graphics::renderer::IRenderer *	m_renderer      = nullptr;
        graphics::renderer::IView *     m_editorView    = nullptr;
        FreeCamEntity *                 m_freeCam       = nullptr;

        core::vector<core::Entity*>     m_entities;

        double                          m_dt = 0.0f;
	};
}