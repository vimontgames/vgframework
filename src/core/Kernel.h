#pragma once

namespace vg::core
{
    class IProfiler;

    class Kernel
    {
    public:
        static void setProfiler(IProfiler * _profiler);
        static IProfiler * getProfiler();

    private:
        static inline IProfiler * s_profiler = nullptr;
    };

    //--------------------------------------------------------------------------------------
    inline void Kernel::setProfiler(IProfiler * _profiler)
    {
        s_profiler = _profiler;
    }

    //--------------------------------------------------------------------------------------
    inline IProfiler * Kernel::getProfiler()
    {
        VG_ASSERT(s_profiler, "Profiler singleton interface pointer is not specified for this executable or dynamic library.");
        return s_profiler;
    }
}