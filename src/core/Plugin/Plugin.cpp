#include "core/Precomp.h"
#include "Plugin.h"
#include "core/Kernel.h"
#include "core/string/string.h"

namespace vg::core
{
#if VG_WINDOWS
	// Returns the string description of a Win32 error
	static string GetWin32ErrorAsString(DWORD errorID)
	{
		if (errorID == 0)
		{
			return std::string();
		}

		LPSTR messageBuffer = nullptr;

		// Call FormatMessage asking to allocate.
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, errorID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

		std::string message(messageBuffer, size);

		LocalFree(messageBuffer);

		return message;
	}
#endif

	//--------------------------------------------------------------------------------------
	IPlugin * Plugin::createInternal(const string & _name, const string & _configSuffix)
	{
		string filename;

		if (_configSuffix.empty())
			filename = fmt::sprintf("bin/%s/%s/%s.%s", getPlatform(), getConfiguration(), _name, getExtension());
		else
			filename = fmt::sprintf("bin/%s/%s_%s/%s.%s", getPlatform(), getConfiguration(), _configSuffix, _name,  getExtension());
		
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
			VG_ASSERT(false, "Error: \"%s\" (0x%08X) loading %s Plugin \"%s\"", GetWin32ErrorAsString(error).c_str(), error, _name.c_str(), filename.c_str());
        }
		#endif

		if (nullptr != instance)
			instance->SetName(filename);
		else
			VG_ERROR("Could not create instance of Plugin \"%s\"", _name.c_str());

		return instance;
	}
	
	//--------------------------------------------------------------------------------------
	string Plugin::getPlatform()
	{
		#ifdef VG_WINDOWS
			#if defined(_M_ARM64EC) 
				return "ARM64EC";
			#elif defined(_M_ARM64)
				return "ARM64";
			#else
				return "x64";
			#endif
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