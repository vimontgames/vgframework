#pragma once

#include "gfx/Resource/Resource.h"

namespace D3D12MA
{
    class Allocation;
}

namespace vg::gfx::vulkan
{
    class Resource : public base::Resource
    {
        using super = base::Resource;

    public:
        Resource();
        ~Resource();

        void                setVulkanImage          (const VkImage & _vkImage, const VmaAllocation & _vmaAlloc, VkDeviceAddress _vkDeviceAddress = VkDeviceAddress(-1));
        void                setVulkanBuffer         (const VkBuffer & _vkBuffer, const VmaAllocation & _vmaAlloc, const VmaAllocationInfo & _vmaAllocInfo, VkDeviceAddress _vkDeviceAddress = VkDeviceAddress(-1));

        VkImage             getVulkanImage          () const    { VG_ASSERT(ResourceType::Texture == m_resourceType); return m_vkImage; }
        VkBuffer            getVulkanBuffer         () const    { VG_ASSERT(ResourceType::Buffer == m_resourceType); return m_vkBuffer; }

        Map                 map                     (core::uint _subResource = 0, Range _read = Range());
        void                unmap                   (core::uint _subResource = 0, Range _write = Range());

        VkDeviceAddress     getVulkanDeviceAddress  () const { VG_ASSERT(VkDeviceAddress(-1) != m_vkDeviceAddress); return m_vkDeviceAddress; }

    private:
        union
        {
            VkImage				m_vkImage;
            VkBuffer            m_vkBuffer;
        };
        VmaAllocation           m_vma_alloc;
        VmaAllocationInfo       m_vmaAllocInfo;
        VkDeviceAddress         m_vkDeviceAddress = VkDeviceAddress(-1);
    };
}