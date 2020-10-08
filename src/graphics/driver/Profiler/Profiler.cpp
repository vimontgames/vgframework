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

using namespace vg::core;

namespace vg::graphics::driver
{
    //--------------------------------------------------------------------------------------
    void Profiler::init()
    {
        Device * device = Device::get();

        #ifdef VG_DX12

        OPTICK_GPU_INIT_D3D12(DEVICE, CMD_QUEUES, NUM_CMD_QUEUS);

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
    void Profiler::setCommandList(CommandList * _cmdList)
    {
        #ifdef VG_DX12
        toudou
        #elif VG_VULKAN
        OPTICK_GPU_CONTEXT(_cmdList->getVulkanCommandBuffer());
        #endif
    }

    //--------------------------------------------------------------------------------------
    void Profiler::swap()
    {
        Device * device = Device::get();
        OPTICK_GPU_FLIP(device->getVulkanSwapchain());
    }

    //--------------------------------------------------------------------------------------
    void Profiler::deinit()
    {
        OPTICK_SHUTDOWN();
    }
}

#endif // VG_ENABLE_OPTICK