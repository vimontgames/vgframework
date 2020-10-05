namespace vg::graphics::driver::vulkan
{
    //--------------------------------------------------------------------------------------
    Buffer::Buffer(const BufferDesc & _bufDesc, const core::string & _name, void * _initData, ReservedSlot _reservedSlot) :
        base::Buffer(_bufDesc, _name, _initData)
    {
        auto * device = driver::Device::get();
        auto & vkDevice = device->getVulkanDevice();

        VkBufferCreateInfo vkBufferCreate = {};
                           vkBufferCreate.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

        VmaAllocationCreateInfo allocCreateInfo = {};

        switch (_bufDesc.resource.m_usage)
        {
            case Usage::Default:
                vkBufferCreate.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                vkBufferCreate.usage = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                vkBufferCreate.size = _bufDesc.size();
                allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
                allocCreateInfo.flags = 0;
                break;

            case Usage::Upload:
                vkBufferCreate.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                vkBufferCreate.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                vkBufferCreate.size = _bufDesc.size();
                allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
                allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
                break;
        }
        
        VkBuffer vkBuffer;
        VmaAllocation vmaAlloc;
        VmaAllocationInfo vmaAllocInfo = {};
        VG_ASSERT_VULKAN(vmaCreateBuffer(device->getVulkanMemoryAllocator(), &vkBufferCreate, &allocCreateInfo, &vkBuffer, &vmaAlloc, &vmaAllocInfo));

        m_resource.setVulkanBuffer(vkBuffer, vmaAlloc, vmaAllocInfo);

        if (asBool(BindFlags::ShaderResource & _bufDesc.resource.m_bindFlags))
        {
            VG_ASSERT(m_resource.getVulkanBuffer());

            VkBufferViewCreateInfo vkBufferViewDesc = {};

            vkBufferViewDesc.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
            vkBufferViewDesc.buffer = m_resource.getVulkanBuffer();
            vkBufferViewDesc.format = VK_FORMAT_R32_UINT;
            vkBufferViewDesc.range = _bufDesc.size();
            
            VG_ASSERT_VULKAN(vkCreateBufferView(device->getVulkanDevice(), &vkBufferViewDesc, nullptr, &m_vkBufferView));

            BindlessTable * bindlessTable = device->getBindlessTable();
            m_bindlessSRVHandle = bindlessTable->allocBindlessBufferHandle(static_cast<driver::Buffer*>(this), _reservedSlot);

            VkDescriptorBufferInfo vkBufferInfo = {};
            vkBufferInfo.buffer = m_resource.getVulkanBuffer();
            vkBufferInfo.offset = 0;
            vkBufferInfo.range = _bufDesc.size();

            VkWriteDescriptorSet writes = {};
            writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes.dstBinding = bindless_buffer_binding;
            writes.descriptorCount = 1;
            writes.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
            writes.pBufferInfo = &vkBufferInfo;
            writes.pTexelBufferView = &m_vkBufferView;
            writes.dstSet = device->m_vkBindlessDescriptors;
            writes.dstArrayElement = m_bindlessSRVHandle - bindless_buffer_offset;

            vkUpdateDescriptorSets(device->getVulkanDevice(), 1, &writes, 0, nullptr);

            if (nullptr != _initData)
            {
                VkMemoryRequirements mem_reqs;
                vkGetBufferMemoryRequirements(device->getVulkanDevice(), getResource().getVulkanBuffer(), &mem_reqs);

                u64 uploadBufferSize = mem_reqs.size;

                auto & context = device->getCurrentFrameContext();
                auto * mapped = context.m_uploadBuffer->getResource().m_vmaAllocInfo.pMappedData;

                // Copy to upload buffer
                context.m_uploadCur = (u8*)alignUp((uint_ptr)context.m_uploadCur, (core::u32)mem_reqs.alignment);
                const u32 offset = (u32)(context.m_uploadCur - context.m_uploadBegin);

                memcpy(context.m_uploadCur, _initData, _bufDesc.size());

                device->upload(static_cast<driver::Buffer*>(this), offset);

                context.m_uploadCur += uploadBufferSize;
            }
        }
    }

    //--------------------------------------------------------------------------------------
    Buffer::~Buffer()
    {
        
    }
}