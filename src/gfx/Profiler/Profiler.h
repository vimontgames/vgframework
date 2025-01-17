#pragma once

#include "core/Singleton/Singleton.h"
#include "core/Kernel.h"

#define VG_ENABLE_OPTICK

#ifdef VG_ENABLE_OPTICK

#pragma push_macro("new")
#undef new
#include "optick/src/optick.h"
#pragma pop_macro("new")

#define VG_PROFILE_CATEGORY(name, cat)  OPTICK_CATEGORY(name, Optick::Category::cat)
#define VG_PROFILE_GPU(name)            OPTICK_GPU_EVENT_DYNAMIC(name); vg::gfx::ScopedGPUEvent scopedGPUEvent##__COUNTER__(name)   
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

    class Profiler final : public core::IProfiler
    {
    public:
        void                                    Init                    () final override;
        void                                    Deinit                  () final override;
        void                                    Start                   () final override;
        void                                    Stop                    () final override;
        bool                                    IsCaptureInProgress     () const final override;

        void                                    StartCpuEvent           (const char * _name) final override;
        void                                    StopCpuEvent            () final override;
        
        void                                    AddCpuEventLabel        (const char * _name, float _data) final override;
        void                                    AddCpuEventLabel        (const char * _name, core::i32 _data) final override;
        void                                    AddCpuEventLabel        (const char * _name, core::u32 _data) final override;
        void                                    AddCpuEventLabel        (const char * _name, core::u64 _data) final override;
        void                                    AddCpuEventLabel        (const char * _name, const core::float3 & _data) final override;
        void                                    AddCpuEventLabel        (const char * _name, const core::string & _data) final override;

        void                                    StartGpuEvent           (const char * _name) final override;
        void                                    StopGpuEvent            () final override;
        void                                    RegisterProfilerThread  (const char * _name) final override;

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
            if (m_name[0] != '\0')
                core::Kernel::getProfiler()->StartGpuEvent(_name);
        }

        inline ~ScopedGPUEvent()
        {
            if (m_name[0] != '\0')
                core::Kernel::getProfiler()->StopGpuEvent();
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

