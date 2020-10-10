#pragma once

#include "core/Singleton/Singleton.h"

#define VG_ENABLE_OPTICK

#ifdef VG_ENABLE_OPTICK
#include "optick/src/optick.h"

#define VG_PROFILE_CATEGORY(name, cat)  OPTICK_CATEGORY(name, Optick::Category::cat)
#define VG_PROFILE_GPU(name)            OPTICK_GPU_EVENT(name);
#define VG_PROFILE_SAVE()               Profiler::save();

#ifdef VG_DX12
#define VG_PROFILE_GPU_SWAP(dev)        OPTICK_GPU_FLIP(dev->getd3d12SwapChain());
#define VG_PROFILE_GPU_CONTEXT(cmd)     OPTICK_GPU_CONTEXT(cmd->getd3d12CommandList())
#elif VG_VULKAN
#define VG_PROFILE_GPU_SWAP(dev)        OPTICK_GPU_FLIP(dev->getVulkanSwapchain());
#define VG_PROFILE_GPU_CONTEXT(cmd)     OPTICK_GPU_CONTEXT(cmd->getVulkanCommandBuffer())
#endif

namespace vg::graphics::driver
{
    class Profiler : public core::IProfiler
    {
    public:
        void init                   () override;
        void deinit                 () override;
        void beginFrame             (const char * _name) override;
        void start                  () override;
        void stop                   () override;
        bool isCaptureInProgress    () const override;
        void startCpuEvent          (const char * _name) override;
        void stopCpuEvent           () override;

    private:
        bool m_isCaptureInProgress = false;
    };
}

#else

#define VG_PROFILE_CATEGORY(name, cat)  __noop
#define VG_PROFILE_GPU(name)            __noop
#define VG_PROFILE_GPU_SWAP(dev)        __noop
#define VG_PROFILE_GPU_CONTEXT(cmd)     __noop
#define VG_PROFILE_SAVE()               __noop

#endif

