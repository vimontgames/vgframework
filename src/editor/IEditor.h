#pragma once

#include "core/IPlugin.h"
#include "Editor_Consts.h"

namespace vg
{
    namespace editor
    {
	    class IEditor : public core::IPlugin
	    {
	    public:
            IEditor(const core::string & _name, core::IObject * _parent) :
                core::IPlugin(_name, _parent)
            {

            }

		    virtual void						Init		        (const core::Singletons & _singletons) = 0;
		    virtual void						Deinit		        () = 0;
                                                
            #ifdef _WIN32                       
            virtual LRESULT CALLBACK            WndProc             (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
            #endif                              
                                                
		    virtual void						RunOneFrame	        () = 0;
	    };
    }
}