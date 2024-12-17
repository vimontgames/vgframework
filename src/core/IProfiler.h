#pragma once

#include "Kernel.h"

#ifndef VG_FINAL
#define VG_ENABLE_PROFILER 1
#endif

#if VG_ENABLE_PROFILER

#define VG_PROFILE_INIT()                   vg::core::Kernel::getProfiler()->init()
#define VG_PROFILE_DEINIT()                 vg::core::Kernel::getProfiler()->deinit()
#define VG_PROFILE_START()                  vg::core::Kernel::getProfiler()->start()
#define VG_PROFILE_STOP()                   vg::core::Kernel::getProfiler()->stop()
#define VG_PROFILE_CAPTURE_IN_PROGRESS()    vg::core::Kernel::getProfiler()->isCaptureInProgress()
#define VG_PROFILE_TRIGGER()                if (Kernel::getProfiler()->isCaptureInProgress()) VG_PROFILE_STOP(); else VG_PROFILE_START();
#define VG_PROFILE_CPU_EVENT_START(name)    if (auto * profiler = vg::core::Kernel::getProfiler(false)) { profiler->startCpuEvent(name); }
#define VG_PROFILE_CPU_EVENT_STOP()         if (auto * profiler = vg::core::Kernel::getProfiler(false)) { profiler->stopCpuEvent(); }
#define VG_PROFILE_CPU(name)                vg::core::ScopedCPUEvent scopedCPUEvent##__COUNTER__(name)
#define VG_PROFILE_REGISTER_THREAD(name)    vg::core::Kernel::registerThread(name)

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
#define VG_PROFILE_REGISTER_THREAD(name)    __noop

#endif

namespace vg::core
{
    class IProfiler
    {
    public:
        virtual void init                   () = 0;
        virtual void deinit                 () = 0;
        virtual void start                  () = 0;
        virtual void stop                   () = 0;
        virtual bool isCaptureInProgress    () const = 0;
        virtual void startCpuEvent          (const char * _name) = 0;
        virtual void stopCpuEvent           () = 0;
        virtual void startGpuEvent          (const char * _name) = 0;
        virtual void stopGpuEvent           () = 0;
        virtual void registerProfilerThread (const char * _name) = 0;

        virtual ~IProfiler() {}
    };

    class ScopedCPUEvent
    {
    public:
        inline ScopedCPUEvent(const char * _name) :
            m_name(_name)
        {
            if (m_name[0] != '\0')
            {
                VG_PROFILE_CPU_EVENT_START(_name);
            }
        }

        inline ~ScopedCPUEvent()
        {
            if (m_name[0] != '\0')
            {
                VG_PROFILE_CPU_EVENT_STOP();
            }
        }

    private:
        const char * m_name = nullptr;
    };
}