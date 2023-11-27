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

namespace vg::gfx
{
    class IView;
}

namespace vg::editor
{
    class IEditor;
}

namespace vg::engine
{
    class FreeCam;
    class ResourceManager;
    class Selection;

	class Engine : public IEngine, public core::Singleton<Engine>
	{
	public:
        using super = IEngine;

		IPlugin::Version				    GetVersion	        () const final override;

										    Engine		        (const core::string & _name, core::IObject * _parent);
										    ~Engine		        ();

        bool                                IsPlaying           () const final override;
        bool                                IsPaused            () const final override;
        void                                Play                () final override;
        void                                Stop                () final override;
        void                                Pause               () final override;
        void                                Resume              () final override;

        const Time &                        GetTime             () const final override;

        const char *                        getClassName        () const final { return "Engine"; }
        bool                                RegisterClasses     () override;
        bool                                UnregisterClasses   ();
        static bool                         registerProperties  (core::IClassDesc & _desc);
        
        static bool                         load                (IObject * _object);
        static bool                         save                (IObject * _object);

        void                                onPropertyChanged   (IObject * _object, const core::IProperty & _prop) override;

        static bool                         createProject       (core::IObject * _engine);
        static bool                         loadProject         (core::IObject * _engine);
        static bool                         saveProject         (core::IObject * _engine);

        #ifdef _WIN32
        LRESULT CALLBACK                    WndProc             (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) final;
        #endif

		void							    init		        (const EngineCreationParams & _params, core::Singletons & _singletons) final;
		void							    deinit		        () override;

        void                                Quit                () final override;
        bool                                IsQuitting          () const final override;

        bool                                loadProject         (const core::string & _path) final;
        bool                                unloadProject       () final;
        IProject *                          getProject          () const final;

        core::IUniverse *                   getCurrentUniverse  () const final;

		void							    RunOneFrame	        () final;

        core::uint2                         getScreenSize       () const final;
        gfx::IView *                        getMainView         () const;

        editor::IEditor *                   GetEditor           () const final override;
		renderer::IRenderer *	            GetRenderer	        () const final override;
        engine::IResourceManager *          GetResourceManager  () const final override;
        core::ISelection *                  GetSelection        () const final override;

    public:
        void                                createEditorScene   ();
        void                                destroyEditorView   ();

        VG_INLINE bool                      isPlaying           () const;
        void                                play                ();
        void                                stop                ();

        VG_INLINE bool                      isPaused            () const;
        void                                pause               ();
        void                                resume              ();

        VG_INLINE const Time &              getTime             () const;

    protected:
        void                                updateDt            ();
        void                                toggleFullscreen    ();

	private:
        core::string                        m_projectPath;
        bool                                m_isPlaying         = false;
        bool                                m_isPaused          = false;
        bool                                m_quit              = false;
        IProject *                          m_project           = nullptr;
        core::Universe *                    m_universe          = nullptr;
        editor::IEditor *                   m_editor            = nullptr;
		renderer::IRenderer *	            m_renderer          = nullptr;
        gfx::IView *                        m_mainView          = nullptr;
        ResourceManager *                   m_resourceManager   = nullptr;
        Selection *                         m_selection         = nullptr;
        Time                                m_time;
	};
}

#if VG_ENABLE_INLINE
#include "Engine.inl"
#endif