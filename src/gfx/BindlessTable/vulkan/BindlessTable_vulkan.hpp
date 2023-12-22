namespace vg::gfx::vulkan
{
    //--------------------------------------------------------------------------------------
    BindlessTable::BindlessTable() :
        super::BindlessTable()
    {
        


    }

    //--------------------------------------------------------------------------------------
    BindlessTable::~BindlessTable()
    {

    }

    //--------------------------------------------------------------------------------------
    void BindlessTable::copyTextureHandle(core::uint _slot, gfx::Texture * _texture)
    {
        auto * device = gfx::Device::get();
        VG_ASSERT(device);

        VkDescriptorImageInfo tex_descs = {};
        tex_descs.imageView = _texture->getVulkanImageView();
        tex_descs.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        tex_descs.sampler = nullptr; 

        VkWriteDescriptorSet writes = {};
        writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes.dstBinding = BINDLESS_TEXTURE_BINDING;
        writes.descriptorCount = 1;
        writes.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        writes.pImageInfo = &tex_descs;
        writes.dstSet = device->m_vkBindlessDescriptors;
        writes.dstArrayElement = _slot; 

        vkUpdateDescriptorSets(device->getVulkanDevice(), 1, &writes, 0, nullptr);
    }

    //--------------------------------------------------------------------------------------
    void BindlessTable::updateBindlessTLASHandle(const BindlessTLASHandle & _handle, const gfx::TLAS * _tlas)
    {
        VG_ASSERT(_handle.isValid());

        auto * device = gfx::Device::get();
        VG_ASSERT(device);

        VkWriteDescriptorSetAccelerationStructureKHR info{};
        info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
        info.accelerationStructureCount = 1;
        info.pAccelerationStructures = &_tlas->getVulkanAccelerationStructure();
        info.pNext = nullptr;

        VkWriteDescriptorSet writes = {};
        writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes.dstSet = device->m_vkBindlessDescriptors;
        writes.dstBinding = BINDLESS_TLAS_BINDING;
        writes.dstArrayElement = _handle - BINDLESS_TLAS_START;
        writes.descriptorCount = 1;
        writes.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
        writes.pBufferInfo = nullptr;
        writes.pImageInfo = nullptr;
        writes.pTexelBufferView = nullptr;
        writes.pNext = &info;
        writes.descriptorCount = 1;

        vkUpdateDescriptorSets(device->getVulkanDevice(), 1, &writes, 0, nullptr);
    }
}