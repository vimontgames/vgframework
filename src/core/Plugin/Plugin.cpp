#include "core/Precomp.h"
#include "Plugin.h"
#include "core/Kernel.h"
#include "core/string/string.h"
#include "core/File/File.h"

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
			NULL, errorID, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPSTR)&messageBuffer, 0, NULL);

		std::string message(messageBuffer, size);

		LocalFree(messageBuffer);

		return message;
	}
#endif

	//--------------------------------------------------------------------------------------
	IPlugin * Plugin::createInternal(const string & _name, const string & _configSuffix)
	{
		string filename;

		const auto platform = getPlatform();
		const auto config = getConfiguration();
		const auto extension = getExtension();

		// First, look for the locally compiled version in the "build" folder
		if (_configSuffix.empty())
			filename = fmt::sprintf("build/bin/%s/%s/%s.%s", platform, config, _name, extension);
		else
			filename = fmt::sprintf("build/bin/%s/%s_%s/%s.%s", platform, config, _configSuffix, _name, extension);

		// If it does not exist, then use the prebuilt version
		if (!io::exists(filename))
		{
            if (_configSuffix.empty())
                filename = fmt::sprintf("bin/%s/%s/%s.%s", platform, config, _name, extension);
            else
                filename = fmt::sprintf("bin/%s/%s_%s/%s.%s", platform, config, _configSuffix, _name, extension);
		}
		
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
			VG_ASSERT(hModule, "Error 0x%08X: %s\nCould not load \"%s\" plugin.\n\nIf you are running from the IDE, please make sure that $(SolutionDir) is your working directory.", 
				error,
				GetWin32ErrorAsString(error).c_str(),
				_name.c_str());
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
		#elif defined(VG_DEVELOPMENT)
		return "Development";
		#elif defined(VG_RELEASE)
		return "Release";
		#elif defined(VG_FINAL)
		return "Final";
		#else
		#error No configuration defined
		#endif
	}

    //--------------------------------------------------------------------------------------
    string Plugin::getCompiler()
	{
		#if VG_CLANG
		return "LLVM - clang-cl";
		#elif VG_MSVC
		return "MSVC";
		#else
		#error No compiler defined
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