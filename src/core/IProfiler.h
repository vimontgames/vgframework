#pragma once

#include "Kernel.h"

#define VG_ENABLE_PROFILER 1

#if VG_ENABLE_PROFILER

#define VG_PROFILE_INIT()                   Kernel::getProfiler()->init()
#define VG_PROFILE_DEINIT()                 Kernel::getProfiler()->deinit()
#define VG_PROFILE_FRAME(name)              Kernel::getProfiler()->beginFrame(name)
#define VG_PROFILE_START()                  Kernel::getProfiler()->start()
#define VG_PROFILE_STOP()                   Kernel::getProfiler()->stop()
#define VG_PROFILE_CAPTURE_IN_PROGRESS()    Kernel::getProfiler()->isCaptureInProgress()
#define VG_PROFILE_TRIGGER()                if (Kernel::getProfiler()->isCaptureInProgress()) VG_PROFILE_STOP(); else VG_PROFILE_START();
#define VG_PROFILE_CPU_EVENT_START(name)    Kernel::getProfiler()->startCpuEvent(name)
#define VG_PROFILE_CPU_EVENT_STOP()         Kernel::getProfiler()->stopCpuEvent()
#define VG_PROFILE_CPU(name)                vg::core::ScopedEvent scopedEvent##__COUNTER__(name)

#else

#define VG_PROFILE_INIT()                   __noop
#define VG_PROFILE_DEINIT()                 __noop
#define VG_PROFILE_FRAME(name)              __noop
#define VG_PROFILE_START()                  __noop
#define VG_PROFILE_STOP()                   __noop
#define VG_PROFILE_CAPTURE_IN_PROGRESS()    __noop
#define VG_PROFILE_TRIGGER()                __noop
#define VG_PROFILE_CPU_EVENT_START          __noop
#define VG_PROFILE_CPU_EVENT_STOP           __noop
#define VG_PROFILE_CPU(name)                __noop

#endif

namespace vg::core
{
    class IProfiler
    {
    public:
        virtual void init                   () = 0;
        virtual void deinit                 () = 0;
        virtual void beginFrame             (const char * _name) = 0;
        virtual void start                  () = 0;
        virtual void stop                   () = 0;
        virtual bool isCaptureInProgress    () const = 0;
        virtual void startCpuEvent          (const char * _name) = 0;
        virtual void stopCpuEvent           () = 0;

        virtual ~IProfiler() {}
    };

    class ScopedEvent
    {
    public:
        inline ScopedEvent(const char * _name) :
            m_name(_name)
        {
            VG_PROFILE_CPU_EVENT_START(_name);
        }

        inline ~ScopedEvent()
        {
            VG_PROFILE_CPU_EVENT_STOP();
        }

    private:
        const char * m_name = nullptr;
    };
}