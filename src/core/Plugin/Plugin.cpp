#include "core/Precomp.h"
#include "Plugin.h"
#include "core/Kernel.h"

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

		VG_INFO("[Plugin] Loading %s", filename.c_str());

		#if VG_WINDOWS
		HMODULE hModule = LoadLibraryExA(filename.c_str(), nullptr, 0);
		if (hModule)
		{
			CreateFunc createFunc = (CreateFunc)GetProcAddress(hModule, "CreateNew");
			if (createFunc)
				instance = createFunc();
		}
        else
        {
            DWORD error = GetLastError();
			VG_ASSERT(false, "Error %u (0x%08X) loading %s Plugin \"%s\"", error, error, _name.c_str(), filename.c_str());
        }
		#endif

		if (nullptr != instance)
			instance->setName(filename);
		else
			VG_ERROR("Could not create instance of Plugin \"%s\"", _name.c_str());

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
		#elif defined(VG_RELEASE)
		return "Release";
		#elif defined(VG_FINAL)
		return "Final";
		#else
		#error No configuration defined
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
	Plugin::Plugin(const core::string & _name, core::IObject * _parent) :
        core::IPlugin(_name, _parent)
	{

	}

	//--------------------------------------------------------------------------------------
	Plugin::~Plugin()
	{

	}
}