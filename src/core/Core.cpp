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
	bool assertmsg(const char * _condition, const char * _func, const char * _file, int _line, bool & _skip)
	{
		return assertmsg(_condition, _func, _file, _line, _skip, nullptr);
	}

    //--------------------------------------------------------------------------------------
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
	bool assertmsg(const char * _condition, const char * _func, const char * _file, int _line, bool & _skip, const char * _format, ...)
	{
		if (!_skip)
		{
            string log = string(_file) + " (" + std::to_string(_line) + ")";
            VG_DEBUGPRINT("%s : %s ", log.c_str(), _condition);
  
			const int bufferSize = 4096;
			char tempBuffer[bufferSize];

			if(_format)
			{
				tempBuffer[0] = 0;
				va_list args;
				va_start(args, _format);
				vsnprintf_s(tempBuffer, bufferSize -1, _format, args);
                VG_DEBUGPRINT("%s", tempBuffer);
				va_end(args);
			}
			else
			{
				tempBuffer[0] = '\0';
			}
            VG_DEBUGPRINT("\n");
            
            string title = "Assertion failed in " + string(basename(_file)) + " (" + std::to_string(_line) + ")";
            string text = string(_condition) + "\n";

            if (_format)
                text += "\n" + string(tempBuffer) + "\n";

            text += "\n\"Yes\" to break into debugger\n\"No\" to ignore once\n\"Cancel\" to ignore always";

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