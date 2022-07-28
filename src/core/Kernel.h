#pragma once

namespace vg::core
{
    class IProfiler;
    class IScheduler;
    class IInput;
    class IFactory;

    struct Singletons
    {
        IProfiler *         profiler    = nullptr;
        IScheduler *        scheduler   = nullptr;
        IInput *            input       = nullptr;
        IFactory *    factory     = nullptr;
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

        static void setFactory(IFactory * _factory);
        static IFactory * getFactory();

        static void setSingletons(const Singletons & _other);
        static Singletons & getSingletons();

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
        VG_ASSERT(s_singletons.profiler, "IProfiler interface is not specified for this executable or dynamic library.");
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
        VG_ASSERT(s_singletons.scheduler, "IScheduler interface is not specified for this executable or dynamic library.");
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
        VG_ASSERT(s_singletons.input, "IInput interface is not specified for this executable or dynamic library.");
        return s_singletons.input;
    }

    //--------------------------------------------------------------------------------------
    inline void Kernel::setFactory(IFactory * _factory)
    {
        s_singletons.factory = _factory;
    }

    //--------------------------------------------------------------------------------------
    inline IFactory * Kernel::getFactory()
    {
        VG_ASSERT(s_singletons.factory, "IFactory interface is not specified for this executable or dynamic library.");
        return s_singletons.factory;
    }

    //--------------------------------------------------------------------------------------
    inline void Kernel::setSingletons(const Singletons & _other)
    {
        s_singletons = _other;
    }

    //--------------------------------------------------------------------------------------
    inline Singletons & Kernel::getSingletons()
    {
        return s_singletons;
    }
}