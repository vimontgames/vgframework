#define VMA_IMPLEMENTATION
#include "VulkanMemoryAllocator/include/vk_mem_alloc.h"

namespace vg::gfx::vulkan
{
    //--------------------------------------------------------------------------------------
    MemoryAllocator::MemoryAllocator() :
        super::MemoryAllocator()
    {
        auto * device = gfx::Device::get();
        VG_ASSERT(device);

        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice = device->getVulkanPhysicalDevice();
        allocatorInfo.device = device->getVulkanDevice();
        allocatorInfo.instance = device->getVulkanInstance();
        allocatorInfo.preferredLargeHeapBlockSize = 0; // default
        allocatorInfo.pAllocationCallbacks = nullptr;
        allocatorInfo.pDeviceMemoryCallbacks = nullptr;

        vmaCreateAllocator(&allocatorInfo, &m_vkAllocator);
    }

    //--------------------------------------------------------------------------------------
    MemoryAllocator::~MemoryAllocator()
    {
        vmaDestroyAllocator(m_vkAllocator);
        m_vkAllocator = nullptr;
    }
}