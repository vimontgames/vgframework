#pragma once

#include "core/Types/Types.h"

namespace vg::core
{
	class IObject
	{
	public:
										IObject		() {}
		virtual							~IObject	() {}

		virtual void					setName		(const string & _name) = 0;
		virtual const string &			getName		() const = 0;

		virtual u32						addRef		() = 0;
		virtual u32						release		() = 0;

		// Added so that we can use the same 'VG_SAFE_RELEASE' macro everywhere
		virtual u32						Release		() = 0;
	};
}