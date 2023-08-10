#pragma once

#include "core/IPlugin.h"
#include "Engine_Consts.h"

namespace vg
{
    class IProject;

    namespace core
    {
        struct Singletons;
        class IUniverse;
    }

    namespace renderer
    {
        class IRenderer;
    }

    namespace editor
    {
        class IEditor;
    }

    namespace engine
    {
        class IResourceManager;

        struct Time
        {
            double m_dt = 0.0f;
        };

	    class IEngine : public core::IPlugin
	    {
	    public:
            IEngine(const core::string & _name, core::IObject * _parent) :
                core::IPlugin(_name, _parent)
            {

            }

		    virtual void						init		        (const EngineParams & _params, core::Singletons & _singletons) = 0;
		    virtual void						deinit		        () = 0;

            virtual void                        Quit                () = 0;
            virtual bool                        IsQuitting          () const = 0;

            // Play/Pause/Stop
            virtual bool                        IsPlaying           () const = 0;
            virtual bool                        IsPaused            () const = 0;
            virtual void                        Play                () = 0;
            virtual void                        Pause               () = 0;
            virtual void                        Stop                () = 0;

            virtual const Time &                GetTime             () const = 0;

            virtual bool                        loadProject         (const core::string & _path) = 0;
            virtual bool                        unloadProject       () = 0;
            virtual IProject *                  getProject          () const = 0;

            virtual core::IUniverse *           getCurrentUniverse  () const = 0;

            virtual core::uint2                 getScreenSize       () const = 0;
                                                
            #ifdef _WIN32                       
            virtual LRESULT CALLBACK            WndProc             (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
            #endif                              
                                                
		    virtual void						RunOneFrame	        () = 0;

            virtual editor::IEditor *           GetEditor           () const = 0;
		    virtual renderer::IRenderer *	    GetRenderer	        () const = 0;
            virtual engine::IResourceManager *  GetResourceManager  () const = 0;
	    };
    }
}