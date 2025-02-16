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
    string Plugin::getDynamicLibraryPath(const string & _name, const string & _configSuffix, bool _precompiledBuild)
	{
		const string rootFolder = _precompiledBuild ? "bin" : "build/bin";

        const auto platform = getPlatform();
        const auto config = getConfiguration();
        const auto extension = getExtension();
		const auto compiler = getCompiler();

		string path;

		#ifdef VG_SHARPMAKE
        if (_configSuffix.empty())
            path = fmt::sprintf("%s/%s/%s %s/%s.%s", rootFolder, platform, config, compiler, _name, extension);
        else
            path = fmt::sprintf("%s/%s/%s %s %s/%s.%s", rootFolder, platform, config, compiler, _configSuffix, _name, extension);
		#else
		if (_configSuffix.empty())
			path = fmt::sprintf("%s/%s/%s/%s.%s", rootFolder, platform, config, _name, extension);
		else
			path = fmt::sprintf("%s/%s/%s_%s/%s.%s", rootFolder, platform, config, _configSuffix, _name, extension);
		#endif

		path = tolower(path);

		return path;
	}

	//--------------------------------------------------------------------------------------
	IPlugin * Plugin::createInternal(const string & _name, const string & _configSuffix)
	{
		string filename;
		string buildFilename;
		string precompiledFilename;

		// First, look for the locally compiled version in the "build" folder
		buildFilename = getDynamicLibraryPath(_name, _configSuffix, false);

		// If it does not exist, then use the prebuilt version
		if (io::exists(buildFilename))
		{
			filename = buildFilename;
		}
		else
		{
			precompiledFilename = getDynamicLibraryPath(_name, _configSuffix, true);
			filename = precompiledFilename;
		}

		VG_ASSERT(io::exists(filename), "Could not find the \"%s\" plugin file in any of the following locations:\n- %s\nor\n- %s\n\nCurrent working directory is \"%s\".\n\nIf you are running from the IDE, please make sure that $(SolutionDir) is your working directory.", _name.c_str(), buildFilename.c_str(), precompiledFilename.c_str(), io::getCurrentWorkingDirectory().c_str());
		
		IPlugin * instance = nullptr;

		VG_INFO("[Plugin] Loading %s", filename.c_str());

		#if VG_WINDOWS
		HMODULE hModule = LoadLibraryExA(filename.c_str(), nullptr, 0);
		if (hModule)
		{
			CreateFunc createFunc = (CreateFunc)GetProcAddress(hModule, "CreateNew");
			VG_ASSERT(nullptr != createFunc, "Could not find function \"CreateNew\" in \"%s\" plugin", filename.c_str());
			if (createFunc)
				instance = createFunc();
		}
        else
        {
            DWORD error = GetLastError();
			VG_ASSERT(hModule, "Error 0x%08X: %s\nCould not load the \"%s\" plugin (\"%s\")", 
				error,
				GetWin32ErrorAsString(error).c_str(),
				_name.c_str(),
				filename.c_str());
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
				#ifdef VG_SHARPMAKE
				return "Win64"; // Sharpmake-generated projects use "Win64" instead of "x64" for 64-bits Windows
				#else
				return "x64";
				#endif
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
		return "ClangCL";
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