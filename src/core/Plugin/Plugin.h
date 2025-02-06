#pragma once

#include "core/IPlugin.h"
#include "core/Types/Types.h"

namespace vg::core
{
	class Plugin : public IPlugin
	{
		using CreateFunc = IPlugin * (*)();

	public:
		template <class T> static T *	create(const string & _name, string _configSuffix = "") { return dynamic_cast<T*>(createInternal(_name, _configSuffix)); }

		static string					getPlatform		();
		static string					getConfiguration();
		static string					getExtension	();

	public:
										Plugin			(const core::string & _name, core::IObject * _parent);
										~Plugin			();

	private:
		static IPlugin *				createInternal	(const string & _name, const string & _configSuffix);
	};
}