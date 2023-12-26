#include "gfx/Precomp.h"
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

#include "core/File/File.h"
#include "core/string/string.h"
#include "core/Plugin/Plugin.h"

#include "gfx/Device/Device.h"
#include "gfx/CommandQueue/CommandQueue.h"
#include "gfx/CommandList/CommandList.h"

using namespace vg::core;

namespace vg::gfx
{
    ProfilerContext Profiler::s_contextTLS;

    //--------------------------------------------------------------------------------------
    void Profiler::init()
    {
        Device * device = Device::get();

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
    void Profiler::start()
    {
        VG_INFO("[Profiler] Start capture");
        m_isCaptureInProgress = true;
        OPTICK_START_CAPTURE();
    }

    //--------------------------------------------------------------------------------------
    void Profiler::stop()
    {
        m_isCaptureInProgress = false;
        OPTICK_STOP_CAPTURE();
        VG_INFO("[Profiler] Stop capture");
        time_t now = time(0);
		struct tm tstruct;
        #if defined(OPTICK_MSVC)
        localtime_s(&tstruct, &now);
        #else
        localtime_r(&now, &tstruct);
        #endif
        char timeStr[80] = {};
		strftime(timeStr, sizeof(timeStr), "%dd%mm%Yy%Hh%Mm%Ss", &tstruct);
        const char * profileFolder = "profile";
        io::createFolder(profileFolder);
        string filename = fmt::sprintf("%s/application_%s_%s_%s_%s.opt", profileFolder, Plugin::getPlatform().c_str(), Plugin::getConfiguration().c_str(), asString(Device::get()->getDeviceParams().api).c_str(), timeStr);
        OPTICK_SAVE_CAPTURE(filename.c_str());
        VG_INFO("[Profiler] Opening capture \"%s\"", filename.c_str());
        string command = fmt::sprintf("start %s", filename.c_str());
        system(command.c_str());
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

    //--------------------------------------------------------------------------------------
    void Profiler::registerProfilerThread(const char * _name)
    {
        Optick::RegisterThread(_name);
    }
}

#endif // VG_ENABLE_OPTICK