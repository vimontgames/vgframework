#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
	class IPlugin : public core::Object
	{
	public:
		struct Version
		{
			core::u16 major;
			core::u16 minor;
		};

		virtual Version getVersion	    () const = 0;
		virtual			~IPlugin	    () {}

        virtual bool    registerClasses () = 0;
	};
}