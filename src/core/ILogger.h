#pragma once

#include "core/Types/Types.h"

#define VG_ENABLE_LOGGER 1

#if VG_ENABLE_LOGGER
#define VG_LOG(level, ...)    vg::core::Kernel::getLogger()->Log(level, __VA_ARGS__)
#else
#define VG_LOG(level, ...)    __noop
#endif

namespace vg::core
{
    enum class Level : core::u8
    {
        Info = 0,
        Warning,
        Error
    };

    struct LogEntry
    {
        Level level;
        string category;
        string message;
    };

    class ILogger
    {
    public:
        virtual                     ~ILogger    () {}

        virtual void                Init        () = 0;
        virtual void                Deinit      () = 0;

        virtual void                Log         (Level _level, const char * _format, ...) = 0;

        virtual void                Clear       () = 0;

        virtual void                Lock        () = 0;
        virtual void                Unlock      () = 0;

        virtual const uint          GetLogCount () const = 0;
        virtual const LogEntry &    GetLog      (uint _index) const = 0;
    };
}