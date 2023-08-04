#pragma once

#include "editor/IEditor.h"
#include "core/Singleton/Singleton.h"

namespace vg::editor
{
	class Editor : public IEditor, public core::Singleton<Editor>
	{
	public:
        using super = IEditor;

		IPlugin::Version				    getVersion	        () const final;

										    Editor		        (const core::string & _name, core::IObject * _parent);
										    ~Editor		        ();

        const char *                        getClassName        () const final { return "Editor"; }
        bool                                registerClasses     () override;
        bool                                unregisterClasses   ();
        static bool                         registerProperties  (core::IClassDesc & _desc);
        
		void							    Init		        () final;
		void							    Deinit		        () override;

		void							    RunOneFrame	        () final;
	};
}

#if VG_ENABLE_INLINE
#include "Editor.inl"
#endif