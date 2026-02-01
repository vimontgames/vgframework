#pragma once

#include "core/Types/Types.h"
#include "core/Types/Enum.h"

#ifndef VG_FINAL
#define VG_ENABLE_LOGGER 1
#endif

#if VG_ENABLE_LOGGER
#define VG_LOG(level, ...)                  vg::core::Kernel::getLogger()->Log(level, nullptr, __VA_ARGS__)
#define VG_LOG_OBJECT(level, object, ...)   vg::core::Kernel::getLogger()->Log(level, object, __VA_ARGS__)
#define VG_LOG_ONCE(level, ...)             do                                  \
	                                        {                                   \
	                                        	static bool done = false;       \
	                                        	if (!done)                      \
                                                {                               \
	                                        		VG_LOG(level, __VA_ARGS__); \
                                                    done = true;                \
                                                }                               \
	                                        }                                   \
	                                        while(0)
#else
#define VG_LOG(level, ...)                  __noop
#define VG_LOG_OBJECT(level, object, ...)   __noop
#define VG_LOG_ONCE(level, ...)             __noop
#endif

#define VG_INFO(...)            VG_LOG(vg::core::Level::Info, __VA_ARGS__)
#define VG_WARNING(...)         VG_LOG(vg::core::Level::Warning, __VA_ARGS__)
#define VG_ERROR(...)           VG_LOG(vg::core::Level::Error, __VA_ARGS__)

#define VG_INFO_OBJECT(object, ...)    VG_LOG_OBJECT(vg::core::Level::Info, object, __VA_ARGS__)
#define VG_WARNING_OBJECT(object, ...) VG_LOG_OBJECT(vg::core::Level::Warning, object, __VA_ARGS__)
#define VG_ERROR_OBJECT(object, ...)   VG_LOG_OBJECT(vg::core::Level::Error, object, __VA_ARGS__)

#define VG_INFO_ONCE(...)       VG_LOG_ONCE(vg::core::Level::Info, __VA_ARGS__)
#define VG_WARNING_ONCE(...)    VG_LOG_ONCE(vg::core::Level::Warning, __VA_ARGS__)
#define VG_ERROR_ONCE(...)      VG_LOG_ONCE(vg::core::Level::Error, __VA_ARGS__)

namespace vg::core
{
    vg_enum_class(vg::core, Level, u8,
        Info = 0,
        Warning,
        Error
    );

    vg_enum_class(vg::core, LevelFlags, u32,
        Info    = 1 << (u32)Level::Info,
        Warning = 1 << (u32)Level::Warning,
        Error   = 1 << (u32)Level::Error
    );

    using UID = core::u32;

    struct LogEntry
    {
        u64 crc;
        Level level;
        string category;
        string message;
        UID uid;
        uint count;
    };

    class IObject;

    class ILogger
    {
    public:
        virtual                     ~ILogger    () {}

        virtual void                Init        () = 0;
        virtual void                Deinit      () = 0;

        virtual void                Log         (Level _level, const IObject * _object, const char * _format, ...) = 0;

        virtual void                Clear       () = 0;

        virtual void                Lock        () = 0;
        virtual void                Unlock      () = 0;

        virtual const uint          GetLogCount () const = 0;
        virtual const LogEntry &    GetLog      (uint _index) const = 0;
    };
}