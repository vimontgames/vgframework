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
        tex_descs.sampler = nullptr; // device->vk_immutableSampler;

        VkWriteDescriptorSet writes = {};
        writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes.dstBinding = 0;// 1;
        writes.descriptorCount = 1;
        writes.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        writes.pImageInfo = &tex_descs;
        writes.dstSet = device->m_vkBindlessDescriptors;
        writes.dstArrayElement = _slot; 

        vkUpdateDescriptorSets(device->getVulkanDevice(), 1, &writes, 0, nullptr);
    }
}