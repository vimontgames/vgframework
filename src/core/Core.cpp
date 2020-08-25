#include "core/Precomp.h"
#include "Core.h"
#include "core/Types/Types.h"
#include "core/Math/Math.h"

namespace vg::core
{
	//--------------------------------------------------------------------------------------
	void debugPrint(const char * _format, ...)
	{
		const int bufferSize = 4096;
		char tempBuffer[bufferSize];

		if(_format)
		{
			tempBuffer[0] = 0;
			va_list args;
			va_start(args, _format);
			vsnprintf_s(tempBuffer, bufferSize -1, _format, args);
		#ifdef _MSC_VER
			OutputDebugStringA(tempBuffer);
		#else
			printf_s(tempBuffer);
		#endif
			va_end(args);
		}
	}

	//--------------------------------------------------------------------------------------
	bool assertmsg(const char * _condition, const char * _file, int _line, bool & _skip)
	{
		return assertmsg(_condition, _file, _line, _skip, nullptr);
	}

	const char * basename(const char * _path)
	{
		const auto slash = strrchr(_path, '/');
		const auto antislash = strrchr(_path, '\\');
		
		if (slash || antislash)
			return max(slash, antislash) + 1;
		else
			return _path;
	}

	//--------------------------------------------------------------------------------------
	bool assertmsg(const char * _condition, const char * _file, int _line, bool & _skip, const char * _format, ...)
	{
		if (!_skip)
		{
			const int bufferSize = 4096;
			char tempBuffer[bufferSize];

			if(_format)
			{
				tempBuffer[0] = 0;
				va_list args;
				va_start(args, _format);
				vsnprintf_s(tempBuffer, bufferSize -1, _format, args);
			#ifdef _MSC_VER
				OutputDebugStringA(tempBuffer);
			#else
				printf_s(tempBuffer);
			#endif
				va_end(args);
			}
			else
			{
				tempBuffer[0] = '\0';
			}

			string title = "VG_ASSERT";
			string text = string(_condition) + "\n\nFile: " + string(basename(_file)) + "\nLine: " + std::to_string(_line) + "\n\n" + string(tempBuffer);

			switch (MessageBoxA(nullptr, text.c_str(), title.c_str(), MB_YESNOCANCEL | MB_ICONWARNING))
			{
				case IDCANCEL:
					_skip = true;
					return false;

				case IDYES:
					return true;

				case IDNO:
					return false;
			}
		}

		return false;
	}
}