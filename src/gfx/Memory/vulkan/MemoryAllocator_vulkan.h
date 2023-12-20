#pragma once

#include "gfx/Memory/MemoryAllocator.h"

namespace vg::gfx::vulkan
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