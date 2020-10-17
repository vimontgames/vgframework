#pragma once

#include "core/Types/Types.h"

namespace vg::core
{
	void debugPrint(const char * _format, ...);

    enum class MessageBoxIcon
    {
        Error = 0,
        Warning,
        Info
    };

    enum class MessageBoxType
    {
        YesNoCancel = 0,
        RetryCancel
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

    core::uint_ptr alignUp(core::uint_ptr _adress, core::u32 _align);

    #if defined(__clang__) || defined(__GNUC__)
    #define VG_THREAD_LOCAL __thread
    #elif defined(_MSC_VER)
    #define VG_THREAD_LOCAL __declspec(thread)
    #else
    #error Can not define VG_THREAD_LOCAL
    #endif
}