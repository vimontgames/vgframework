#pragma once

namespace vg::core
{
    class IProfiler;
    class IScheduler;
    class IInput;

    struct Singletons
    {
        IProfiler *     profiler  = nullptr;
        IScheduler *    scheduler = nullptr;
        IInput *        input     = nullptr;
    };

    class Kernel
    {
    public:
        static void setProfiler(IProfiler * _profiler);
        static IProfiler * getProfiler();

        static void setScheduler(IScheduler * _scheduler);
        static IScheduler * getScheduler();

        static void setInput(IInput * _input);
        static IInput * getInput();

    private:
        static inline Singletons s_singletons;
    };

    //--------------------------------------------------------------------------------------
    inline void Kernel::setProfiler(IProfiler * _profiler)
    {
        s_singletons.profiler = _profiler;
    }

    //--------------------------------------------------------------------------------------
    inline IProfiler * Kernel::getProfiler()
    {
        VG_ASSERT(s_singletons.profiler, "Profiler singleton interface pointer is not specified for this executable or dynamic library.");
        return s_singletons.profiler;
    }

    //--------------------------------------------------------------------------------------
    inline void Kernel::setScheduler(IScheduler * _scheduler)
    {
        s_singletons.scheduler = _scheduler;
    }

    //--------------------------------------------------------------------------------------
    inline IScheduler * Kernel::getScheduler()
    {
        VG_ASSERT(s_singletons.scheduler, "Profiler singleton interface pointer is not specified for this executable or dynamic library.");
        return s_singletons.scheduler;
    }

    //--------------------------------------------------------------------------------------
    inline void Kernel::setInput(IInput * _input)
    {
        s_singletons.input = _input;
    }

    //--------------------------------------------------------------------------------------
    inline IInput * Kernel::getInput()
    {
        VG_ASSERT(s_singletons.input, "Input singleton interface pointer is not specified for this executable or dynamic library.");
        return s_singletons.input;
    }
}