#pragma once

#include "core/IPlugin.h"
#include "Engine_Consts.h"

namespace vg
{
    class IGame;

    namespace core
    {
        struct Singletons;
        struct Time;
        class IWorld;
        class ISelection;
    }

    namespace renderer
    {
        class IRenderer;
    }

    namespace physics
    {
        class IPhysics;
    }

    namespace audio
    {
        class IAudio;
    }

    namespace editor
    {
        class IEditor;
    }

    namespace core
    {
        class IResourceManager;
    }

    namespace engine
    {
        class IWorldResource;
        class IEngineOptions;

	    class IEngine : public core::IPlugin
	    {
	    public:
            IEngine(const core::string & _name, core::IObject * _parent) :
                core::IPlugin(_name, _parent)
            {

            }

		    virtual void						        Init		        (EngineCreationParams & _params, core::Singletons & _singletons) = 0;
		    virtual void						        Deinit		        () = 0;

            virtual bool                                IsPlaying           () const = 0;
            virtual bool                                IsPaused            () const = 0;

            virtual void                                Play                () = 0;
            virtual void                                Stop                () = 0;
            virtual void                                Pause               () = 0;
            virtual void                                Resume              () = 0;

            virtual void                                StartInPlayMode     (bool _enable) = 0;

            virtual void                                Quit                () = 0;
            virtual bool                                IsQuitting          () const = 0;

            virtual const core::Time &                  GetTime             () const = 0;

            virtual bool                                LoadGame            (const core::string & _path) = 0;
            virtual bool                                UnloadGame          () = 0;
            virtual IGame *                             GetGame             () const = 0;

            virtual core::IWorld *                      GetMainWorld        () const = 0;
            virtual const core::vector<core::IWorld*> & GetWorlds           () const = 0;

            virtual core::uint2                         GetScreenSize       () const = 0;
                                                        
            #ifdef _WIN32                               
            virtual LRESULT CALLBACK                    WndProc             (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
            #endif                                      
                                                        
		    virtual void						        RunOneFrame	        () = 0;
            virtual void                                FlushLoading        () = 0;

            virtual editor::IEditor *                   GetEditor           () const = 0;

		    virtual renderer::IRenderer *	            GetRenderer	        () const = 0;
            virtual physics::IPhysics *                 GetPhysics          () const = 0;
            virtual audio::IAudio *                     GetAudio            () const = 0;
            virtual core::IResourceManager *            GetResourceManager  () const = 0;
            virtual core::ISelection *                  GetSelection        () const = 0;

            virtual bool                                CreateWorld         (const core::string & _filename) = 0;
            virtual bool                                SaveWorld           () = 0;
            virtual bool                                SaveWorldAs         (const core::string & _filename) = 0;
            virtual bool                                LoadWorld           (const core::string & _filename) = 0;
            virtual IWorldResource *                    GetWorldResource    () = 0;

            virtual bool                                SaveScene           (core::IResource * _sceneRes) = 0;

            virtual IEngineOptions *                    GetOptions          () = 0;
	    };
    }
}