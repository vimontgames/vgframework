#include "core/Precomp.h"
#include "Plugin.h"

namespace vg::core
{
	//--------------------------------------------------------------------------------------
	IPlugin * Plugin::createInternal(const string & _name, const string & _suffix)
	{
		string filename;

		if (_suffix.empty()) 
			filename = _name + "_" + getPlatform() + "_" + getConfiguration() + "." + getExtension();
		else
			filename = _name + "_" + getPlatform() + "_" + getConfiguration() + "_" + _suffix  + "." + getExtension();
		
		IPlugin * instance = nullptr;

		VG_DEBUGPRINT("Loading %s ... ", filename.c_str());

		#if VG_WINDOWS
		HMODULE hModule = LoadLibraryExA(filename.c_str(), nullptr, 0);
		if (hModule)
		{
			CreateFunc createFunc = (CreateFunc)GetProcAddress(hModule, "Create");
			if (createFunc)
				instance = createFunc();
		}
		#endif

		if (instance)
		{
			VG_DEBUGPRINT("OK\n");
			instance->setName(filename);
		}
		else
		{
			VG_DEBUGPRINT("ERROR\n");
		}

		return instance;
	}
	
	//--------------------------------------------------------------------------------------
	string Plugin::getPlatform()
	{
		#ifdef VG_WINDOWS
		return "x64";
		#endif
	}

	//--------------------------------------------------------------------------------------
	string Plugin::getConfiguration()
	{
		#ifdef VG_DEBUG
		return "Debug";
		#else
		return "Release";
		#endif
	}

	//--------------------------------------------------------------------------------------
	string Plugin::getExtension()
	{
		#ifdef VG_WINDOWS
		return "dll";
		#endif
	}

	//--------------------------------------------------------------------------------------
	Plugin::Plugin()
	{

	}

	//--------------------------------------------------------------------------------------
	Plugin::~Plugin()
	{

	}
}