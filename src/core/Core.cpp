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
		#ifdef VG_WINDOWS
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
        const bool isErrorMsg = !strcmp(_condition, "false");

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
            
            string title;
            string text;
            
            if (isErrorMsg)
            {
                title = "Error in " + string(basename(_file)) + " (" + std::to_string(_line) + ")";
            }
            else
            {
                title = "Assertion failed in " + string(basename(_file)) + " (" + std::to_string(_line) + ")";

                text = string(_condition) + "\n";
                if (_format)
                    text += "\n";
            }

            if (_format)
                text += string(tempBuffer) + "\n";

            text += "\n\"Yes\" to break into debugger\n\"No\" to ignore once\n\"Cancel\" to ignore always";

			switch (messageBox(isErrorMsg ? MessageBoxIcon::Error : MessageBoxIcon::Warning, MessageBoxType::YesNoCancel, title.c_str(), text.c_str()))
			{
                default:
                case MessageBoxResult::Yes:
                    return true;

                case MessageBoxResult::No:
                    return false;

                case MessageBoxResult::Cancel:
                    _skip = true;
                    return false;
			}
		}

		return false;
	}

    //--------------------------------------------------------------------------------------
    MessageBoxResult messageBoxF(MessageBoxIcon _icon, MessageBoxType _type, const char * _title, const char * _format, ...)
    {
        const int bufferSize = 4096;
        char tempBuffer[bufferSize];

        if (_format)
        {
            tempBuffer[0] = 0;
            va_list args;
            va_start(args, _format);
            vsnprintf_s(tempBuffer, bufferSize - 1, _format, args);
            VG_DEBUGPRINT("%s", tempBuffer);
            va_end(args);
        }
        else
        {
            tempBuffer[0] = '\0';
        }

        return messageBox(_icon, _type, _title, tempBuffer);
    }

    //--------------------------------------------------------------------------------------
    MessageBoxResult messageBox(MessageBoxIcon _icon, MessageBoxType _type, const char * _title, const char * _message)
    {
        uint icon = 0;
        switch (_icon)
        {
            default:
                VG_ASSERT(false);

            case MessageBoxIcon::Error:
                icon = MB_ICONERROR;
                break;

            case MessageBoxIcon::Warning:
                icon = MB_ICONWARNING;
                break;

            case MessageBoxIcon::Info:
                icon = MB_ICONINFORMATION;
                break;
        }

        uint type = 0;
        switch (_type)
        {
            default:
                VG_ASSERT(false);

            case MessageBoxType::OK:
                type = MB_OK;
                break;

            case MessageBoxType::YesNo:
                type = MB_YESNO;
                break;

            case MessageBoxType::YesNoCancel:
                type = MB_YESNOCANCEL;
                break;  
        }

        switch (MessageBoxA(nullptr, _message, _title, type | icon))
        {
            default:
                VG_ASSERT(false);

            case IDRETRY:
                return MessageBoxResult::Retry;

            case IDCANCEL:
                return MessageBoxResult::Cancel;

            case IDYES:
                return MessageBoxResult::Yes;

            case IDNO:
                return MessageBoxResult::No;
        }
    }

    //--------------------------------------------------------------------------------------
    MessageBoxResult messageBox(MessageBoxIcon _icon, MessageBoxType _type, const string & _title, const string & _message)
    {
        return messageBox(_icon, _type, _title.c_str(), _message.c_str());
    }

    //--------------------------------------------------------------------------------------
    uint_ptr alignUp(uint_ptr _adress, u32 _align)
    {
        VG_ASSERT((0 != _align) && !(_align & (_align - 1)));
        return (uint_ptr(_adress + (_align - 1)) & ~(uint_ptr)(_align - 1));
    }
}