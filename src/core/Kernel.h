#pragma once

namespace vg::core
{
    class ILogger;
    class IProfiler;
    class IScheduler;
    class IInput;
    class IFactory;
    class ISelection;
    class IResourceManager;
    class IUndoRedoManager;

    struct Singletons
    {
        ILogger *           logger          = nullptr;
        IProfiler *         profiler        = nullptr;
        IScheduler *        scheduler       = nullptr;
        IInput *            input           = nullptr;
        IFactory *          factory         = nullptr;
        ISelection *        selection       = nullptr;
        IResourceManager *  resourceManager = nullptr;
        IUndoRedoManager *  undoRedoManager = nullptr;
    };

    class Kernel
    {
    public:
        static void setLogger(ILogger * _logger);
        static ILogger * getLogger();

        static void setProfiler(IProfiler * _profiler);
        static IProfiler * getProfiler(bool _mustExist = true);

        static void setScheduler(IScheduler * _scheduler);
        static IScheduler * getScheduler();

        static void setInput(IInput * _input);
        static IInput * getInput();

        static void setFactory(IFactory * _factory);
        static IFactory * getFactory(bool _mustExist = true);

        static void setSelection(ISelection * _selection);
        static ISelection * getSelection(bool _mustExist = true);

        static void setResourceManager(IResourceManager * _resourceManager);
        static IResourceManager * getResourceManager(bool _mustExist = true);

        static void setUndoRedoManager(IUndoRedoManager * _undoRedoManager);
        static IUndoRedoManager * getUndoRedoManager(bool _mustExist = true);

        static void setSingletons(const Singletons & _other);
        static Singletons & getSingletons();

    private:
        static inline Singletons s_singletons;
    };

    //--------------------------------------------------------------------------------------
    inline void Kernel::setLogger(ILogger * _logger)
    {
        s_singletons.logger = _logger;
    }

    //--------------------------------------------------------------------------------------
    inline ILogger * Kernel::getLogger()
    {
        return s_singletons.logger;
    }

    //--------------------------------------------------------------------------------------
    inline void Kernel::setProfiler(IProfiler * _profiler)
    {
        s_singletons.profiler = _profiler;
    }

    //--------------------------------------------------------------------------------------
    inline IProfiler * Kernel::getProfiler(bool _mustExist)
    {
        VG_ASSERT(!_mustExist || s_singletons.profiler, "IProfiler interface is not specified for this executable or dynamic library.");
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
    inline IFactory * Kernel::getFactory(bool _mustExist)
    {
        VG_ASSERT(!_mustExist || s_singletons.factory, "IFactory interface is not specified for this executable or dynamic library.");
        return s_singletons.factory;
    }

    //--------------------------------------------------------------------------------------
    inline void Kernel::setSelection(ISelection * _selection)
    {
        s_singletons.selection = _selection;
    }

    //--------------------------------------------------------------------------------------
    inline ISelection * Kernel::getSelection(bool _mustExist)
    {
        VG_ASSERT(!_mustExist || s_singletons.selection, "ISelection interface is not specified for this executable or dynamic library.");
        return s_singletons.selection;
    }

    //--------------------------------------------------------------------------------------
    inline void Kernel::setResourceManager(IResourceManager * _resourceManager)
    {
        s_singletons.resourceManager = _resourceManager;
    }

    //--------------------------------------------------------------------------------------
    inline IResourceManager * Kernel::getResourceManager(bool _mustExist)
    {
        return s_singletons.resourceManager;
    }

    //--------------------------------------------------------------------------------------
    inline void Kernel::setUndoRedoManager(IUndoRedoManager * _undoRedoManager)
    {
        s_singletons.undoRedoManager = _undoRedoManager;
    }

    //--------------------------------------------------------------------------------------
    inline IUndoRedoManager * Kernel::getUndoRedoManager(bool _mustExist)
    {
        return s_singletons.undoRedoManager;
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