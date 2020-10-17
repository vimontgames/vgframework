#include "graphics/driver/Precomp.h"
#include "Profiler.h"

#ifdef VG_ENABLE_OPTICK

#include "optick/src/optick_core.cpp"
#include "optick/src/optick_capi.cpp"
#include "optick/src/optick_gpu.cpp"
#include "optick/src/optick_message.cpp"
#include "optick/src/optick_miniz.cpp"
#include "optick/src/optick_serialization.cpp"
#include "optick/src/optick_server.cpp"

#ifdef VG_DX12
#include "optick/src/optick_gpu.d3d12.cpp"
#elif defined(VG_VULKAN)
#include "optick/src/optick_gpu.vulkan.cpp"
#endif

#include "graphics/driver/Device/Device.h"
#include "graphics/driver/CommandQueue/CommandQueue.h"
#include "graphics/driver/CommandList/CommandList.h"
#include "core/Plugin/Plugin.h"

using namespace vg::core;

namespace vg::graphics::driver
{
    ProfilerContext Profiler::s_contextTLS;

    //--------------------------------------------------------------------------------------
    void Profiler::init()
    {
        Device * device = Device::get();

        OPTICK_APP("VGFramework");

        #ifdef VG_DX12

        vector<ID3D12CommandQueue*> d3d12Queues;

        for (uint i = 0; i < enumCount<CommandQueueType>(); ++i)
        {
            const auto queueType = (CommandQueueType)i;
            CommandQueue * queue = device->getCommandQueue(queueType);

            if (queue)
                d3d12Queues.push_back(queue->getd3d12CommandQueue());
        }

        OPTICK_GPU_INIT_D3D12(device->getd3d12Device(), d3d12Queues.data(), (uint)d3d12Queues.size());

        #elif defined(VG_VULKAN)
 
        vector<VkQueue> vkQueues;
        vector<u32> vkQueuesFamily;

        for (uint i = 0; i < enumCount<CommandQueueType>(); ++i)
        {
            const auto queueType = (CommandQueueType)i;
            CommandQueue * queue = device->getCommandQueue(queueType);
        
            if (queue)
            {
                vkQueues.push_back(queue->getVulkanCommandQueue());
                vkQueuesFamily.push_back(device->getVulkanCommandQueueFamilyIndex(queueType));
            }
        }

        OPTICK_GPU_INIT_VULKAN(&device->getVulkanDevice(), &device->getVulkanPhysicalDevice(),(VkQueue*)vkQueues.data(), vkQueuesFamily.data(), (uint)vkQueues.size(), nullptr);

        #endif
    }

    //--------------------------------------------------------------------------------------
    void Profiler::deinit()
    {
        OPTICK_SHUTDOWN();
    }

    //--------------------------------------------------------------------------------------
    void Profiler::beginFrame(const char * _name)
    {
        OPTICK_FRAME(_name);
    }

    //--------------------------------------------------------------------------------------
    void Profiler::start()
    {
        VG_DEBUGPRINT("Start profile capture\n");
        m_isCaptureInProgress = true;
        OPTICK_START_CAPTURE();
    }

    //--------------------------------------------------------------------------------------
    void Profiler::stop()
    {
        m_isCaptureInProgress = false;
        OPTICK_STOP_CAPTURE();
        VG_DEBUGPRINT("Stop profile capture\n");
        time_t now = time(0);
		struct tm tstruct;
        #if defined(OPTICK_MSVC)
        localtime_s(&tstruct, &now);
        #else
        localtime_r(&now, &tstruct);
        #endif
        char timeStr[80] = {};
		strftime(timeStr, sizeof(timeStr), "%dd%mm%Yy%Hh%Mm%Ss", &tstruct);
        char filename[256] = {};
        sprintf(filename, "vgframework_%s_%s_%s_%s.opt", Plugin::getPlatform().c_str(), Plugin::getConfiguration().c_str(), asString(Device::get()->getDeviceParams().api).c_str(), timeStr);
        OPTICK_SAVE_CAPTURE(filename);
        VG_DEBUGPRINT("Save and open profile capture \"%s\"\n", filename);
        char command[256];
        sprintf(command, "start %s", filename);
        system(command);
    }

    //--------------------------------------------------------------------------------------
    bool Profiler::isCaptureInProgress() const
    {
        return m_isCaptureInProgress;
    }

    //--------------------------------------------------------------------------------------
    void Profiler::startCpuEvent(const char * _name)
    {
        OPTICK_PUSH_DYNAMIC(_name);
    }

    //--------------------------------------------------------------------------------------
    void Profiler::stopCpuEvent()
    {
        OPTICK_POP();
    }

    //--------------------------------------------------------------------------------------
    void Profiler::startGpuEvent(const char * _name)
    {
        #ifdef VG_ENABLE_GPU_MARKER
        getCommandList()->beginGPUEvent(_name, 0xFFFFFFFF);
        #endif
    }

    //--------------------------------------------------------------------------------------
    void Profiler::stopGpuEvent()
    {
        #ifdef VG_ENABLE_GPU_MARKER
        getCommandList()->endGPUEvent();
        #endif
    }
}

#endif // VG_ENABLE_OPTICK