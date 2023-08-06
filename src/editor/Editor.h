#pragma once

#include "editor/IEditor.h"
#include "core/Singleton/Singleton.h"

namespace vg::editor
{
	class Editor : public IEditor, public core::Singleton<Editor>
	{
	public:
        using super = IEditor;

		IPlugin::Version				    GetVersion	        () const final override;

										    Editor		        (const core::string & _name, core::IObject * _parent);
										    ~Editor				();

		bool								RegisterClasses		() final override;
        bool								UnregisterClasses	() final override;

        #ifdef _WIN32
        LRESULT CALLBACK                    WndProc             (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) final override;
        #endif

		void							    Init		        (const core::Singletons & _singletons) final override;
		void							    Deinit		        () final override;

		void							    DrawGUI				(const GUIContext & _context) final override;
	};
}

#if VG_ENABLE_INLINE
#include "Editor.inl"
#endif