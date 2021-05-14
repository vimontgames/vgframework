#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
	class IPlugin : public core::ObjectPointer
	{
	public:
		struct Version
		{
			core::u16 major;
			core::u16 minor;
		};

        IPlugin(const core::string & _name, core::IObject * _parent) :
            core::ObjectPointer(_name, _parent)
        {

        }

		virtual Version getVersion	    () const = 0;
		virtual			~IPlugin	    () {}

        virtual bool    registerClasses () = 0;
	};
}