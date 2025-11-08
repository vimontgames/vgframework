#pragma push_macro("new")
#undef new
#define VMA_IMPLEMENTATION
#include "VulkanMemoryAllocator/include/vk_mem_alloc.h"
#pragma pop_macro("new")

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

        allocatorInfo.flags = 0x0;

        // RayTracing acceleration structure require device address
        if (device->getDeviceCaps().vulkan.deviceAddress)
            allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

        vmaCreateAllocator(&allocatorInfo, &m_vkAllocator);
    }

    //--------------------------------------------------------------------------------------
    MemoryAllocator::~MemoryAllocator()
    {
        vmaDestroyAllocator(m_vkAllocator);
        m_vkAllocator = nullptr;
    }
}