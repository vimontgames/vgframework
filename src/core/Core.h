#pragma once

#include "core/Types/Types.h"
#include "core/ILogger.h"

namespace vg::core
{
	void debugPrint(const char * _format, ...);

    enum class MessageBoxIcon
    {
        Error = 0,
        Warning,
        Info
    };

    enum MessageBoxType
    {
        OK = 0,
        YesNo,
        YesNoCancel
    };

    enum class MessageBoxResult
    {
        Yes = 0,
        No,
        Cancel,
        Retry
    };

    MessageBoxResult messageBoxF(MessageBoxIcon _icon, MessageBoxType _type, const char * _title, const char * _format, ...);
    MessageBoxResult messageBox(MessageBoxIcon _icon, MessageBoxType _type, const char * _title, const char * _message);
    MessageBoxResult messageBox(MessageBoxIcon _icon, MessageBoxType _type, const core::string & _title, const core::string & _message);

    bool assertmsg(const char * _condition, const char * _func, const char * _file, int _line, bool & _skip, const char * _format, ...);
    bool assertmsg(const char * _condition, const char * _func, const char * _file, int _line, bool & _skip);

    #if defined(__clang__) || defined(__GNUC__)
    #define VG_THREAD_LOCAL __thread
    #elif defined(VG_WINDOWS)
    #define VG_THREAD_LOCAL __declspec(thread)
    #else
    #error Can not define VG_THREAD_LOCAL
    #endif
}