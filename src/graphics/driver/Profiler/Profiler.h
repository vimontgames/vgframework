#pragma once

#include "IProfiler.h"
#include "core/Singleton/Singleton.h"

#define VG_ENABLE_OPTICK

#ifdef VG_ENABLE_OPTICK
#include "optick/src/optick.h"

#define VG_PROFILE_INIT()               Profiler::init()
#define VG_PROFILE_DEINIT()             Profiler::deinit()

#define VG_PROFILE_START()              OPTICK_START_CAPTURE()
#define VG_PROFILE_STOP()               OPTICK_STOP_CAPTURE()

#define VG_PROFILE_CATEGORY(name, cat)  OPTICK_CATEGORY(name, Optick::Category::cat)
#define VG_PROFILE_CPU(name)            OPTICK_EVENT(name);
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
    class Profiler : public IProfiler, public core::Singleton<Profiler>
    {
    public:
        static void init();
        static void deinit();
        static void save();
    };
}

#else

#define VG_PROFILE_INIT()               __noop
#define VG_PROFILE_DEINIT()             __noop
#define VG_PROFILE_START()              __noop
#define VG_PROFILE_STOP()               __noop
#define VG_PROFILE_CATEGORY(name, cat)  __noop
#define VG_PROFILE_CPU(name)            __noop
#define VG_PROFILE_GPU(name)            __noop
#define VG_PROFILE_GPU_SWAP(dev)        __noop
#define VG_PROFILE_GPU_CONTEXT(cmd)     __noop

#endif

