namespace vg::graphics::driver::vulkan
{
    //--------------------------------------------------------------------------------------
    Shader::Shader(const core::Blob & _bytecode) :
        super::Shader(_bytecode)
    {
        VkShaderModuleCreateInfo vkModuleDesc;

        vkModuleDesc.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vkModuleDesc.pNext = nullptr;
        vkModuleDesc.flags = 0;
        vkModuleDesc.codeSize = _bytecode.size() * sizeof(u32);
        vkModuleDesc.pCode = (const u32*)_bytecode.data();

        VG_ASSERT_VULKAN(vkCreateShaderModule(driver::Device::get()->getVulkanDevice(), &vkModuleDesc, nullptr, &m_vkShaderModule));
    }

    //--------------------------------------------------------------------------------------
    Shader::~Shader()
    {
        vkDestroyShaderModule(driver::Device::get()->getVulkanDevice(), m_vkShaderModule, nullptr);
    }
}