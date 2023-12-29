#pragma once

#include "core/Types/Types.h"

#define VG_ENABLE_LOGGER 1

#if VG_ENABLE_LOGGER
#define VG_LOG(level, ...)    vg::core::Kernel::getLogger()->Log(level, __VA_ARGS__)
#define VG_LOG_ONCE(level, ...)         \
	do                                  \
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
#define VG_LOG(level, ...)      __noop
#define VG_LOG_ONCE(level, ...) __noop
#endif

#define VG_INFO(...)         VG_LOG(vg::core::Level::Info, __VA_ARGS__)
#define VG_WARNING(...)      VG_LOG(vg::core::Level::Warning, __VA_ARGS__)
#define VG_ERROR(...)        VG_LOG(vg::core::Level::Error, __VA_ARGS__)

#define VG_INFO_ONCE(...)    VG_LOG_ONCE(vg::core::Level::Info, __VA_ARGS__)
#define VG_WARNING_ONCE(...) VG_LOG_ONCE(vg::core::Level::Warning, __VA_ARGS__)
#define VG_ERROR_ONCE(...)   VG_LOG_ONCE(vg::core::Level::Error, __VA_ARGS__)

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
        u64 crc;
        Level level;
        string category;
        string message;
        uint count;
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