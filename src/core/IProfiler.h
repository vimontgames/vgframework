#pragma once

#include "Kernel.h"

#ifndef VG_FINAL
#define VG_ENABLE_PROFILER 1
#endif

#if VG_ENABLE_PROFILER

#define VG_PROFILE_INIT()                       vg::core::Kernel::getProfiler()->Init()
#define VG_PROFILE_DEINIT()                     vg::core::Kernel::getProfiler()->Deinit()
#define VG_PROFILE_START()                      vg::core::Kernel::getProfiler()->Start()
#define VG_PROFILE_STOP()                       vg::core::Kernel::getProfiler()->Stop()
#define VG_PROFILE_CAPTURE_IN_PROGRESS()        vg::core::Kernel::getProfiler()->IsCaptureInProgress()
#define VG_PROFILE_TRIGGER()                    if (Kernel::getProfiler()->IsCaptureInProgress()) VG_PROFILE_STOP(); else VG_PROFILE_START();
#define VG_PROFILE_CPU_EVENT_START(name)        if (auto * profiler = vg::core::Kernel::getProfiler(false)) { profiler->StartCpuEvent(name); }
#define VG_PROFILE_CPU_EVENT_STOP()             if (auto * profiler = vg::core::Kernel::getProfiler(false)) { profiler->StopCpuEvent(); }
#define VG_PROFILE_CPU(name)                    vg::core::ScopedCPUEvent scopedCPUEvent##__COUNTER__(name)
#define VG_PROFILE_CPU_LABEL(name, value)       vg::core::Kernel::getProfiler()->AddCpuEventLabel(name, value)
#define VG_PROFILE_REGISTER_THREAD(name)        vg::core::Kernel::RegisterThread(name)

#else

#define VG_PROFILE_INIT()                       __noop
#define VG_PROFILE_DEINIT()                     __noop
#define VG_PROFILE_FRAME(name)                  __noop
#define VG_PROFILE_START()                      __noop
#define VG_PROFILE_STOP()                       __noop
#define VG_PROFILE_CAPTURE_IN_PROGRESS()        __noop
#define VG_PROFILE_TRIGGER()                    __noop
#define VG_PROFILE_CPU_EVENT_START              __noop
#define VG_PROFILE_CPU_EVENT_STRING_START       __noop
#define VG_PROFILE_CPU_EVENT_STOP               __noop
#define VG_PROFILE_CPU_EVENT_STRING_STOP        __noop
#define VG_PROFILE_CPU(name)                    __noop
#define VG_PROFILE_CPU_LABEL(name, value)       __noop
#define VG_PROFILE_REGISTER_THREAD(name)        __noop

#endif

namespace vg::core
{
    class IProfiler
    {
    public:
        virtual void Init                       () = 0;
        virtual void Deinit                     () = 0;
        virtual void Start                      () = 0;
        virtual void Stop                       () = 0;
        virtual bool IsCaptureInProgress        () const = 0;

        virtual void StartCpuEvent              (const char * _name) = 0;
        virtual void StartCpuEvent              (const core::string & _name) = 0;
        virtual void StopCpuEvent               () = 0;

        virtual void AddCpuEventLabel           (const char * _name, float _data) = 0;
        virtual void AddCpuEventLabel           (const char * _name, core::i32 _data) = 0;
        virtual void AddCpuEventLabel           (const char * _name, core::u32 _data) = 0;
        virtual void AddCpuEventLabel           (const char * _name, core::u64 _data) = 0;
        virtual void AddCpuEventLabel           (const char * _name, const core::float3 & _data) = 0;
        virtual void AddCpuEventLabel           (const char * _name, const core::string & _data) = 0;

        virtual void StartGpuEvent              (const char * _name) = 0;
        virtual void StartGpuEvent              (const core::string & _name) = 0;
        virtual void StopGpuEvent               () = 0;

        virtual void RegisterProfilerThread     (const char * _name) = 0;
        virtual void UnregisterProfilerThread   () = 0;

        virtual ~IProfiler() {}
    };

#if VG_ENABLE_PROFILER
    class ScopedCPUEvent
    {
    public:
        //--------------------------------------------------------------------------------------
        inline ScopedCPUEvent(const char * _name)
        {
            VG_ASSERT(nullptr != _name);
            if (_name[0] != '\0')
            {
                VG_PROFILE_CPU_EVENT_START(_name);
                m_started = true;
            }
        }

        //--------------------------------------------------------------------------------------
        inline ScopedCPUEvent(const core::string & _name)
        {
            if (!_name.empty())
            {
                VG_PROFILE_CPU_EVENT_START(_name);
                m_started = true;
            }
        }

        //--------------------------------------------------------------------------------------
        inline ~ScopedCPUEvent()
        {
            if (m_started)
                VG_PROFILE_CPU_EVENT_STOP();
        }

    private:
        bool m_started = false;
    };
#endif
}