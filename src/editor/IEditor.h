#pragma once

#include "core/IPlugin.h"
#include "Editor_Consts.h"

namespace vg::editor
{
    class IResourceManager;

	class IEditor : public core::IPlugin
	{
	public:
        IEditor(const core::string & _name, core::IObject * _parent) :
            core::IPlugin(_name, _parent)
        {

        }

		virtual void							    Init		        () = 0;
		virtual void							    Deinit		        () = 0;                                   
                                                    
		virtual void							    RunOneFrame	        () = 0;
	};
}