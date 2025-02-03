#pragma once

#include "core/Types/Types.h"
#include "core/ILogger.h"

vg_enum_class_ns(vg::core, MessageBoxIcon, u8,
    Error = 0,
    Warning,
    Info
);

vg_enum_ns(vg::core, MessageBoxType, u8,
    OK = 0,
    YesNo,
    YesNoCancel
);

vg_enum_class_ns(vg::core, MessageBoxResult, u8,
    Yes = 0,
    No,
    Cancel,
    Retry
);

namespace vg::core
{
	void debugPrint(const char * _format, ...);

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