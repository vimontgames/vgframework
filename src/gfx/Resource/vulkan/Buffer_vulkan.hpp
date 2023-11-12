namespace vg::gfx::vulkan
{
    //--------------------------------------------------------------------------------------
    Buffer::Buffer(const BufferDesc & _bufDesc, const core::string & _name, const void * _initData, ReservedSlot _reservedSlot) :
        base::Buffer(_bufDesc, _name, _initData)
    {
        auto * device = gfx::Device::get();
        auto & vkDevice = device->getVulkanDevice();

        VkBufferCreateInfo vkBufferCreate = {};
        vkBufferCreate.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

        VmaAllocationCreateInfo allocCreateInfo = {};

        switch (_bufDesc.resource.m_usage)
        {
            default:
                VG_ASSERT(false, "Unhandled Usage \"%s\" (%u)", asString(_bufDesc.resource.m_usage).c_str(), _bufDesc.resource.m_usage);
                break;

            case Usage::Default:
                vkBufferCreate.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                vkBufferCreate.usage = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                vkBufferCreate.size = _bufDesc.size();
                allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
                allocCreateInfo.flags = 0;
                break;

            case Usage::Upload:
                vkBufferCreate.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                vkBufferCreate.usage = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                vkBufferCreate.size = _bufDesc.size();
                allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
                allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
                break;

            case Usage::Staging:
                vkBufferCreate.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                vkBufferCreate.usage = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                vkBufferCreate.size = _bufDesc.size();
                allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
                allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
                break;
        }

        if (asBool(_bufDesc.resource.m_bindFlags & BindFlags::IndexBuffer))
            vkBufferCreate.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

        if (asBool(_bufDesc.resource.m_bindFlags & BindFlags::UnorderedAccess))
            vkBufferCreate.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;

        VkBuffer vkBuffer;
        VmaAllocation vmaAlloc;
        VmaAllocationInfo vmaAllocInfo = {};
        VG_ASSERT_VULKAN(vmaCreateBuffer(device->getVulkanMemoryAllocator(), &vkBufferCreate, &allocCreateInfo, &vkBuffer, &vmaAlloc, &vmaAllocInfo));

        m_resource.setVulkanBuffer(vkBuffer, vmaAlloc, vmaAllocInfo);

        if (_bufDesc.testBindFlags(BindFlags::ShaderResource | BindFlags::IndexBuffer))
        {
            VG_ASSERT(m_resource.getVulkanBuffer());

            VkBufferViewCreateInfo vkBufferViewDesc = {};

            vkBufferViewDesc.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
            vkBufferViewDesc.buffer = m_resource.getVulkanBuffer();
            vkBufferViewDesc.format = (_bufDesc.testBindFlags(BindFlags::IndexBuffer) & (_bufDesc.elementSize == 2)) ? VK_FORMAT_R16_UINT : VK_FORMAT_R32_UINT;
            vkBufferViewDesc.range = _bufDesc.size();

            VG_ASSERT_VULKAN(vkCreateBufferView(device->getVulkanDevice(), &vkBufferViewDesc, nullptr, &m_vkBufferView));

            BindlessTable * bindlessTable = device->getBindlessTable();
            m_bufferHandle = bindlessTable->allocBindlessBufferHandle(static_cast<gfx::Buffer *>(this), _reservedSlot);

            VkDescriptorBufferInfo vkBufferInfo = {};
            vkBufferInfo.buffer = m_resource.getVulkanBuffer();
            vkBufferInfo.offset = 0;
            vkBufferInfo.range = _bufDesc.size();

            VkWriteDescriptorSet writes = {};
            writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes.dstBinding = BINDLESS_BUFFER_BINDING;
            writes.descriptorCount = 1;
            writes.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writes.pBufferInfo = &vkBufferInfo;
            writes.pTexelBufferView = &m_vkBufferView;
            writes.dstSet = device->getVulkanBindlessDescriptors();
            writes.dstArrayElement = m_bufferHandle - BINDLESS_BUFFER_START;

            vkUpdateDescriptorSets(device->getVulkanDevice(), 1, &writes, 0, nullptr);

            if (nullptr != _initData)
            {
                VkMemoryRequirements mem_reqs;
                vkGetBufferMemoryRequirements(device->getVulkanDevice(), getResource().getVulkanBuffer(), &mem_reqs);

                u64 uploadBufferSize = mem_reqs.size;

                auto * uploadBuffer = device->getUploadBuffer();
                u8 * dst = uploadBuffer->map(uploadBufferSize, (uint)mem_reqs.alignment);
                if (nullptr != dst)
                {
                    // Copy to upload buffer
                    memcpy(dst, _initData, _bufDesc.size());
                }
                uploadBuffer->unmap(static_cast<gfx::Buffer*>(this), dst);
            }
        }

        if (asBool(BindFlags::UnorderedAccess & _bufDesc.resource.m_bindFlags))
        {
            BindlessTable * bindlessTable = device->getBindlessTable();
            m_rwBufferHandle = bindlessTable->allocBindlessRWBufferHandle(static_cast<gfx::Buffer *>(this));

            VkDescriptorBufferInfo vkBufferInfo = {};
            vkBufferInfo.buffer = m_resource.getVulkanBuffer();
            vkBufferInfo.offset = 0;
            vkBufferInfo.range = _bufDesc.size();

            VkWriteDescriptorSet writes = {};
            writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes.dstBinding = BINDLESS_RWBUFFER_BINDING;
            writes.descriptorCount = 1;
            writes.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writes.pBufferInfo = &vkBufferInfo;
            writes.pTexelBufferView = &m_vkBufferView;
            writes.dstSet = device->getVulkanBindlessDescriptors();
            writes.dstArrayElement = m_rwBufferHandle - BINDLESS_RWBUFFER_START;

            vkUpdateDescriptorSets(device->getVulkanDevice(), 1, &writes, 0, nullptr);
        }
    }

    //--------------------------------------------------------------------------------------
    Buffer::~Buffer()
    {
        
    }
}