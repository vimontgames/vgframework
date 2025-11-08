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
                vkBufferCreate.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                vkBufferCreate.size = _bufDesc.getSize();
                allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
                allocCreateInfo.flags = 0;
                break;

            case Usage::Upload:
                vkBufferCreate.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                vkBufferCreate.usage = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                vkBufferCreate.size = _bufDesc.getSize();
                allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
                allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
                break;

            case Usage::Staging:
                vkBufferCreate.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                vkBufferCreate.usage = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                vkBufferCreate.size = _bufDesc.getSize();
                allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
                allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
                break;
        }

        if (_bufDesc.testBindFlags(BindFlags::IndexBuffer))
            vkBufferCreate.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

        if (_bufDesc.testBindFlags(BindFlags::UnorderedAccess))
            vkBufferCreate.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;

        if (_bufDesc.testBindFlags(BindFlags::RaytracingAccelerationStruct))
            vkBufferCreate.usage |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
        else if (device->getDeviceCaps().rayTracing.supported)
            vkBufferCreate.usage |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;

        if (device->getDeviceCaps().vulkan.deviceAddress)
            vkBufferCreate.usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

        VkBuffer vkBuffer;
        VmaAllocation vmaAlloc;
        VmaAllocationInfo vmaAllocInfo = {};

        uint alignment = _bufDesc.getAlignment();

        if (alignment == 0)
        {
            VG_VERIFY_VULKAN(vmaCreateBuffer(device->getVulkanMemoryAllocator(), &vkBufferCreate, &allocCreateInfo, &vkBuffer, &vmaAlloc, &vmaAllocInfo));
        }
        else
        {
            VG_VERIFY_VULKAN(vmaCreateBufferWithAlignment(device->getVulkanMemoryAllocator(), &vkBufferCreate, &allocCreateInfo, (VkDeviceSize)alignment, &vkBuffer, &vmaAlloc, &vmaAllocInfo));
        }

        VkDeviceAddress vkDeviceAddress = VkDeviceAddress(-1);
        if (VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT & vkBufferCreate.usage)
        {
            VkBufferDeviceAddressInfo bufferAddressInfo = {};
            bufferAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
            bufferAddressInfo.pNext = nullptr;
            bufferAddressInfo.buffer = vkBuffer;
            vkDeviceAddress = device->getBufferDeviceAddress(&bufferAddressInfo);
        }

        m_resource.setVulkanBuffer(vkBuffer, vmaAlloc, vmaAllocInfo, vkDeviceAddress);

        if (_bufDesc.testBindFlags(BindFlags::ShaderResource | BindFlags::IndexBuffer))
        {
            VG_ASSERT(m_resource.getVulkanBuffer());

            VkBufferViewCreateInfo vkBufferViewDesc = {};

            vkBufferViewDesc.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
            vkBufferViewDesc.buffer = m_resource.getVulkanBuffer();
            vkBufferViewDesc.format = (_bufDesc.testBindFlags(BindFlags::IndexBuffer | BindFlags::ShaderResource) & (_bufDesc.elementSize == 2)) ? VK_FORMAT_R16_UINT : VK_FORMAT_R32_UINT;
            vkBufferViewDesc.range = _bufDesc.getSize();

            VG_VERIFY_VULKAN(vkCreateBufferView(device->getVulkanDevice(), &vkBufferViewDesc, nullptr, &m_vkBufferView));

            BindlessTable * bindlessTable = device->getBindlessTable();
            m_bufferHandle = bindlessTable->allocBindlessBufferHandle(static_cast<gfx::Buffer *>(this), _reservedSlot);

            VkDescriptorBufferInfo vkBufferInfo = {};
            vkBufferInfo.buffer = m_resource.getVulkanBuffer();
            vkBufferInfo.offset = 0;
            vkBufferInfo.range = _bufDesc.getSize();

            VkWriteDescriptorSet writes = {};
            writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes.dstBinding = BINDLESS_BUFFER_BINDING;
            writes.descriptorCount = 1;
            writes.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writes.pBufferInfo = &vkBufferInfo;
            writes.pTexelBufferView = &m_vkBufferView;
            writes.dstSet = device->getVulkanBindlessDescriptors();
            writes.dstArrayElement = m_bufferHandle - BINDLESS_BUFFER_START;

            vkUpdateDescriptorSets(device->getVulkanDevice(), 1, &writes, 0, nullptr);

            if (nullptr != _initData)
            {
                VkMemoryRequirements mem_reqs;
                vkGetBufferMemoryRequirements(device->getVulkanDevice(), getResource().getVulkanBuffer(), &mem_reqs);
                VG_ASSERT(_bufDesc.getSize() <= mem_reqs.size);
                u64 uploadBufferSize = _bufDesc.getSize();
                const auto * scheduler = Kernel::getScheduler();
                VG_ASSERT(scheduler->IsMainThread() || scheduler->IsLoadingThread(), "Expected Main or Loading thread but current thread is \"%s\"", scheduler->GetCurrentThreadName().c_str());
                auto * uploadBuffer = device->getUploadBuffer(0);
                u8 * dst = uploadBuffer->map((gfx::Buffer *)this, uploadBufferSize, (uint)mem_reqs.alignment);
                if (nullptr != dst)
                {
                    // Copy to upload buffer
                    memcpy(dst, _initData, uploadBufferSize);
                }
                uploadBuffer->unmap((gfx::Buffer*)this, dst, uploadBufferSize);
            }
        }

        if (asBool(BindFlags::UnorderedAccess & _bufDesc.resource.m_bindFlags))
        {
            BindlessTable * bindlessTable = device->getBindlessTable();
            m_rwBufferHandle = bindlessTable->allocBindlessRWBufferHandle(static_cast<gfx::Buffer *>(this), _reservedSlot);

            VkDescriptorBufferInfo vkBufferInfo = {};
            vkBufferInfo.buffer = m_resource.getVulkanBuffer();
            vkBufferInfo.offset = 0;
            vkBufferInfo.range = _bufDesc.getSize();

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

        // TODO : differentiate BLAS from TLAS and create descriptor only for TLAS?
        if (_bufDesc.testBindFlags(BindFlags::RaytracingAccelerationStruct))
        {
            //BindlessTable * bindlessTable = device->getBindlessTable();
            //m_tlasHandle = bindlessTable->allocBindlessTLASHandle(static_cast<gfx::Buffer *>(this), _reservedSlot);
            //
            //VkWriteDescriptorSetAccelerationStructureKHR vkASInfo = {};
            //vkASInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
            //vkASInfo.accelerationStructureCount = 1;
            //vkASInfo.pAccelerationStructures = &static_cast<const TLASInternal *>(pTLAS)->GetHandle();
            //vkASInfo.pNext = nullptr;

            //vkBufferInfo.buffer = m_resource.getVulkanBuffer();
            //vkBufferInfo.offset = 0;
            //vkBufferInfo.range = _bufDesc.getSize();
            //
            //VkWriteDescriptorSet writes = {};
            //writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            //writes.dstBinding = BINDLESS_BUFFER_BINDING;
            //writes.descriptorCount = 1;
            //writes.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            //writes.pBufferInfo = &vkBufferInfo;
            //writes.pTexelBufferView = &m_vkBufferView;
            //writes.dstSet = device->getVulkanBindlessDescriptors();
            //writes.dstArrayElement = m_bufferHandle - BINDLESS_BUFFER_START;
            //
            //vkUpdateDescriptorSets(device->getVulkanDevice(), 1, &writes, 0, nullptr);
        }
    }

    //--------------------------------------------------------------------------------------
    Buffer::~Buffer()
    {
        
    }
}