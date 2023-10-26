namespace vg::gfx::vulkan
{
    //--------------------------------------------------------------------------------------
    Resource::Resource()
    {

    }

    //--------------------------------------------------------------------------------------
    Resource::~Resource()
    {
        Device * device = gfx::Device::get();

        switch (m_resourceType)
        {
            default:
                VG_ASSERT(false, "Unhandled ResourceType \"%s\" (%u)", asString(m_resourceType), m_resourceType);
                break;
            case ResourceType::Texture:
                if (!testResourceFlags(ResourceFlags::Backbuffer))
                    vkDestroyImage(device->getVulkanDevice(), m_vkImage, nullptr);
                break;
            case ResourceType::Buffer:
                vkDestroyBuffer(device->getVulkanDevice(), m_vkBuffer, nullptr);
                break;
        }

        vmaFreeMemory(device->getVulkanMemoryAllocator(), m_vma_alloc);
    }

    //--------------------------------------------------------------------------------------
    void Resource::setVulkanImage(const VkImage & _vkImage, const VmaAllocation & _vmaAlloc)
    {
        m_resourceType = ResourceType::Texture;
        m_vkImage = _vkImage;
        m_vma_alloc = _vmaAlloc;
    }

    //--------------------------------------------------------------------------------------
    void Resource::setVulkanBuffer(const VkBuffer & _vkBuffer, const VmaAllocation & _vmaAlloc, const VmaAllocationInfo & _vmaAllocInfo)
    {
        m_resourceType = ResourceType::Buffer;
        m_vkBuffer = _vkBuffer;
        m_vma_alloc = _vmaAlloc;
        m_vmaAllocInfo = _vmaAllocInfo;
    }

    //--------------------------------------------------------------------------------------
    Map Resource::map(core::uint _subResource, Range _read)
    {
        Device * device = gfx::Device::get();
        auto * allocator = device->getVulkanMemoryAllocator();

        Map result;
        VG_ASSERT_VULKAN(vmaMapMemory(allocator, m_vma_alloc, &result.data));

        //if (1) // flush for read
        //    vmaFlushAllocation(allocator, m_vma_alloc, _read.begin, _read.end - _read.begin);

        return result;
    }

    //--------------------------------------------------------------------------------------
    void Resource::unmap(core::uint _subResource, Range _write)
    {
        Device * device = gfx::Device::get();
        auto * allocator = device->getVulkanMemoryAllocator();

        //if (1) // flush for write
        //    vmaFlushAllocation(allocator, m_vma_alloc, _write.begin, _write.end - _write.begin);
        
        vmaUnmapMemory(allocator, m_vma_alloc);
    }
}