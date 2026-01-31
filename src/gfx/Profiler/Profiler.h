#pragma once

#include "core/Singleton/Singleton.h"
#include "core/Kernel.h"

#if VG_ENABLE_PROFILER
#define VG_ENABLE_OPTICK
#endif

#ifdef VG_ENABLE_OPTICK

#pragma push_macro("new")
#undef new
#include "optick/src/optick.h"
#pragma pop_macro("new")

//--------------------------------------------------------------------------------------
inline const char * getGpuEventString(const char * _value)
{
    return _value;
}

//--------------------------------------------------------------------------------------
inline const char * getGpuEventString(const vg::core::string & _value)
{
    return _value.c_str();
}

#define VG_PROFILE_CATEGORY(name, cat)  OPTICK_CATEGORY(name, Optick::Category::cat)
#define VG_PROFILE_GPU(name)            { const char * staticName = getGpuEventString(name); OPTICK_GPU_EVENT_DYNAMIC(staticName); vg::gfx::ScopedGPUEvent scopedGPUEvent##__COUNTER__(staticName); }
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
        void                                    Init                        () final override;
        void                                    Deinit                      () final override;
        void                                    Start                       () final override;
        void                                    Stop                        () final override;
        bool                                    IsCaptureInProgress         () const final override;

        void                                    StartCpuEvent               (const char * _name) final override;
        void                                    StartCpuEvent               (const core::string & _name) final override;
        void                                    StopCpuEvent                () final override;
        
        void                                    AddCpuEventLabel            (const char * _name, float _data) final override;
        void                                    AddCpuEventLabel            (const char * _name, core::i32 _data) final override;
        void                                    AddCpuEventLabel            (const char * _name, core::u32 _data) final override;
        void                                    AddCpuEventLabel            (const char * _name, core::u64 _data) final override;
        void                                    AddCpuEventLabel            (const char * _name, const core::float3 & _data) final override;
        void                                    AddCpuEventLabel            (const char * _name, const core::string & _data) final override;

        void                                    StartGpuEvent               (const char * _name) final override;
        void                                    StartGpuEvent               (const core::string & _name) final override;
        void                                    StopGpuEvent                () final override;
        void                                    RegisterProfilerThread      (const char * _name) final override;
        void                                    UnregisterProfilerThread    () final override;

        static void                             setCommandList              (CommandList * _cmdList) { s_contextTLS.cmdList = _cmdList; }
        static CommandList *                    getCommandList              () { auto * cmdList = s_contextTLS.cmdList; VG_ASSERT(cmdList); return cmdList; }

    private:
        static VG_THREAD_LOCAL ProfilerContext  s_contextTLS;
        bool                                    m_isCaptureInProgress  = false;
    };

    class ScopedGPUEvent
    {
    public:

        //--------------------------------------------------------------------------------------
        inline ScopedGPUEvent(const char * _name)
        {
            VG_ASSERT(nullptr != _name);
            if (_name[0] != '\0')
            {
                core::Kernel::getProfiler()->StartGpuEvent(_name);
                m_started = true;
            }
        }

        //--------------------------------------------------------------------------------------
        //inline ScopedGPUEvent(const core::string & _name)
        //{
        //    if (!_name.empty())
        //    {
        //        core::Kernel::getProfiler()->StartGpuEvent(_name);
        //        m_started = true;
        //    }
        //}

        //--------------------------------------------------------------------------------------
        inline ~ScopedGPUEvent()
        {
            if (m_started)
                core::Kernel::getProfiler()->StopGpuEvent();
        }

    private:
        bool m_started = false;
    };
}

#else

#define VG_PROFILE_CATEGORY(name, cat)  __noop
#define VG_PROFILE_GPU(name)            __noop
#define VG_PROFILE_GPU_SWAP(dev)        __noop
#define VG_PROFILE_GPU_CONTEXT(cmd)     __noop
#define VG_PROFILE_SAVE()               __noop

#endif

