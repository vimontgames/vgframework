#pragma once

#include "core/IPlugin.h"

namespace vg
{
    namespace editor
    {
        struct GUIContext
        {
            void * ptr = nullptr;
        };

        class IEditorOptions;

	    class IEditor : public core::IPlugin
	    {
	    public:
            IEditor(const core::string & _name, core::IObject * _parent) :
                core::IPlugin(_name, _parent)
            {

            }

		    virtual void						Init		        (const core::Singletons & _singletons) = 0;
		    virtual void						Deinit		        () = 0;

            virtual void                        RunOneFrame         (float _dt) = 0;
                                                
            #ifdef _WIN32                       
            virtual LRESULT CALLBACK            WndProc             (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
            #endif                              
                                                
		    virtual void						DrawGUI	            (const GUIContext & _context) = 0;

            virtual IEditorOptions *            GetOptions          () const = 0;
	    };
    }
}