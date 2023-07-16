#pragma once

#include "engine/IEngine.h"
#include "core/Singleton/Singleton.h"
#include "core/Resource/Resource.h"

namespace vg::core
{
    class Universe;
    class Scene;
    class Entity;
}

namespace vg::graphics::renderer
{
    class IView;
}

namespace vg::engine
{
    class FreeCam;
    class ResourceManager;

	class Engine : public IEngine, public core::Singleton<Engine>
	{
	public:
        using super = IEngine;

		IPlugin::Version				    getVersion	        () const final;

										    Engine		        (const core::string & _name, core::IObject * _parent);
										    ~Engine		        ();

        const char *                        getClassName        () const final { return "Engine"; }
        bool                                registerClasses     () override;
        bool                                unregisterClasses   ();
        static bool                         registerProperties  (core::IClassDesc & _desc);
        
        static bool                         load                (IObject * _object);
        static bool                         save                (IObject * _object);

        void                                onPropertyChanged   (const core::IProperty & _prop) override;

        static bool                         createProject       (core::IObject * _engine);
        static bool                         loadProject         (core::IObject * _engine);
        static bool                         saveProject         (core::IObject * _engine);

        #ifdef _WIN32
        LRESULT CALLBACK                    WndProc             (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) final;
        #endif

		void							    init		        (const EngineParams & _params, core::Singletons & _singletons) final;
		void							    deinit		        () override;

        bool                                loadProject         (const core::string & _path) final;
        bool                                unloadProject       () final;
        IProject *                          getProject          () const final;

        core::IUniverse *                   getCurrentUniverse  () const final;

		void							    runOneFrame	        () final;

        core::uint2                         getScreenSize       () const final;
        graphics::renderer::IView *         getMainView         () const;

		graphics::renderer::IRenderer *	    GetRenderer	        () const final;
        engine::IResourceManager *          GetResourceManager  () const final;

    public:
        void                                createEditorScene   ();
        void                                destroyEditorView   ();

    protected:
        void                                updateDt            ();

	private:
        core::string                        m_projectPath;

        IProject *                          m_project           = nullptr;
        core::Universe *                    m_universe          = nullptr;
		graphics::renderer::IRenderer *	    m_renderer          = nullptr;
        graphics::renderer::IView *         m_mainView          = nullptr;
        ResourceManager *                   m_resourceManager   = nullptr;

        double                              m_dt                = 0.0f;
	};
}