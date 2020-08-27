#pragma once

#include "graphics/driver/Memory/MemoryAllocator.h"
#include "VulkanMemoryAllocator/src/vk_mem_alloc.h"

namespace vg::graphics::driver::vulkan
{
    class MemoryAllocator : public base::MemoryAllocator
    {
        using super = base::MemoryAllocator;

    public:
        MemoryAllocator();
        ~MemoryAllocator();

        VmaAllocator & getVulkanMemoryAllocator() { return m_vkAllocator; }

    private:
        VmaAllocator m_vkAllocator;
    };
}