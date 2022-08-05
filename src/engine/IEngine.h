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

    namespace graphics::renderer
    {
        class IRenderer;
    }
}

namespace vg::engine
{
    class IResourceManager;

	class IEngine : public core::IPlugin
	{
	public:
        IEngine(const core::string & _name, core::IObject * _parent) :
            core::IPlugin(_name, _parent)
        {

        }

		virtual void							    init		        (const EngineParams & _params, core::Singletons & _singletons) = 0;
		virtual void							    deinit		        () = 0;

        virtual bool                                loadProject         (const core::string & _path) = 0;
        virtual bool                                unloadProject       () = 0;
        virtual IProject *                          getProject          () const = 0;

        virtual core::IUniverse *                   getCurrentUniverse  () const = 0;

        virtual core::uint2                         getScreenSize       () const = 0;
                                                    
        #ifdef _WIN32                               
        virtual LRESULT CALLBACK                    WndProc             (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
        #endif                                      
                                                    
		virtual void							    runOneFrame	        () = 0;

		virtual graphics::renderer::IRenderer *	    GetRenderer	        () const = 0;
        virtual engine::IResourceManager *          GetResourceManager  () const = 0;
	};
}