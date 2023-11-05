#pragma once

#include "core/Singleton/Singleton.h"
#include "core/Kernel.h"

#define VG_ENABLE_OPTICK

#ifdef VG_ENABLE_OPTICK
#include "optick/src/optick.h"

#define VG_PROFILE_CATEGORY(name, cat)  OPTICK_CATEGORY(name, Optick::Category::cat)
#define VG_PROFILE_GPU(name)            OPTICK_GPU_EVENT_DYNAMIC(name); ScopedGPUEvent scopedGPUEvent##__COUNTER__(name)
#define VG_PROFILE_SAVE()               Profiler::save();

#ifdef VG_DX12
#define VG_PROFILE_GPU_SWAP(dev)        OPTICK_GPU_FLIP(dev->getd3d12SwapChain())
#define VG_PROFILE_GPU_CONTEXT(cmd)     OPTICK_GPU_CONTEXT(cmd->getd3d12CommandList()) Profiler::setCommandList(cmd)
#elif VG_VULKAN
#define VG_PROFILE_GPU_SWAP(dev)        OPTICK_GPU_FLIP(dev->getVulkanSwapchain());
#define VG_PROFILE_GPU_CONTEXT(cmd)     OPTICK_GPU_CONTEXT(cmd->getVulkanCommandBuffer()) Profiler::setCommandList(cmd)
#endif

namespace vg::gfx
{
    class CommandList;

    struct ProfilerContext
    {
        CommandList * cmdList = nullptr;
    };

    class Profiler : public core::IProfiler
    {
    public:
        void                                    init                    () override;
        void                                    deinit                  () override;
        void                                    start                   () override;
        void                                    stop                    () override;
        bool                                    isCaptureInProgress     () const override;
        void                                    startCpuEvent           (const char * _name) override;
        void                                    stopCpuEvent            () override;
        void                                    startGpuEvent           (const char * _name) override;
        void                                    stopGpuEvent            () override;
        void                                    registerProfilerThread  (const char * _name) override;

        static void                             setCommandList          (CommandList * _cmdList) { s_contextTLS.cmdList = _cmdList; }
        static CommandList *                    getCommandList          () { auto * cmdList = s_contextTLS.cmdList; VG_ASSERT(cmdList); return cmdList; }

    private:
        static VG_THREAD_LOCAL ProfilerContext  s_contextTLS;
        bool                                    m_isCaptureInProgress  = false;
    };

    class ScopedGPUEvent
    {
    public:
        inline ScopedGPUEvent(const char * _name) :
            m_name(_name)
        {
            core::Kernel::getProfiler()->startGpuEvent(_name);
        }

        inline ~ScopedGPUEvent()
        {
            core::Kernel::getProfiler()->stopGpuEvent();
        }

    private:
        const char * m_name = nullptr;
    };
}

#else

#define VG_PROFILE_CATEGORY(name, cat)  __noop
#define VG_PROFILE_GPU(name)            __noop
#define VG_PROFILE_GPU_SWAP(dev)        __noop
#define VG_PROFILE_GPU_CONTEXT(cmd)     __noop
#define VG_PROFILE_SAVE()               __noop

#endif

