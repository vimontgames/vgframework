namespace vg::graphics::driver::vulkan
{
    //--------------------------------------------------------------------------------------
    Buffer::Buffer(const BufferDesc & _bufDesc, const core::string & _name, void * _initData) :
        base::Buffer(_bufDesc, _name, _initData)
    {
        auto * device = driver::Device::get();
        auto & vkDevice = device->getVulkanDevice();

        VkBufferCreateInfo bufDesc = {};
        VmaAllocationCreateInfo allocCreateInfo = {};

        if (Usage::Upload == _bufDesc.resource.m_usage)
        {
            bufDesc.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufDesc.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            bufDesc.size = _bufDesc.size();
            
            allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
        }

        VkBuffer vkBuffer;
        VmaAllocation vmaAlloc;
        VG_ASSERT_VULKAN(vmaCreateBuffer(device->getVulkanMemoryAllocator(), &bufDesc, &allocCreateInfo, &vkBuffer, &vmaAlloc, nullptr));
        m_resource.setVulkanBuffer(vkBuffer, vmaAlloc);
    }

    //--------------------------------------------------------------------------------------
    Buffer::~Buffer()
    {
        
    }
}