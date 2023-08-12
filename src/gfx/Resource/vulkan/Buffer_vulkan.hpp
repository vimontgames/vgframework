namespace vg::gfx::vulkan
{
    //--------------------------------------------------------------------------------------
    Buffer::Buffer(const BufferDesc & _bufDesc, const core::string & _name, const void * _initData, ReservedSlot _reservedSlot) :
        base::Buffer(_bufDesc, _name, _initData)
    {
        auto * device = gfx::Device::get();
        auto & vkDevice = device->getVulkanDevice();

        if (!_bufDesc.testUsageFlags(Usage::Dynamic))
        {
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
                    vkBufferCreate.usage = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                    vkBufferCreate.size = _bufDesc.size();
                    allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
                    allocCreateInfo.flags = 0;
                    break;

                case Usage::Upload:
                    vkBufferCreate.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                    vkBufferCreate.usage = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                    vkBufferCreate.size = _bufDesc.size();
                    allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
                    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
                    break;

                case Usage::Dynamic:
                    break;
            }

            if (asBool(_bufDesc.resource.m_bindFlags & BindFlags::IndexBuffer))
                vkBufferCreate.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

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
                vkBufferViewDesc.format = VK_FORMAT_R32_UINT;
                vkBufferViewDesc.range = _bufDesc.size();

                VG_ASSERT_VULKAN(vkCreateBufferView(device->getVulkanDevice(), &vkBufferViewDesc, nullptr, &m_vkBufferView));

                BindlessTable * bindlessTable = device->getBindlessTable();
                m_bindlessSRVHandle = bindlessTable->allocBindlessBufferHandle(static_cast<gfx::Buffer*>(this), _reservedSlot);

                VkDescriptorBufferInfo vkBufferInfo = {};
                vkBufferInfo.buffer = m_resource.getVulkanBuffer();
                vkBufferInfo.offset = 0;
                vkBufferInfo.range = _bufDesc.size();

                VkWriteDescriptorSet writes = {};
                writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writes.dstBinding = BINDLESS_TEXTURE_SRV_BINDING;
                writes.descriptorCount = 1;
                writes.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
                writes.pBufferInfo = &vkBufferInfo;
                writes.pTexelBufferView = &m_vkBufferView;
                writes.dstSet = device->getVulkanBindlessDescriptors();
                writes.dstArrayElement = m_bindlessSRVHandle - BINDLESS_TEXTURE_SRV_OFFSET;

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
        }
    }

    //--------------------------------------------------------------------------------------
    Buffer::~Buffer()
    {
        
    }
}