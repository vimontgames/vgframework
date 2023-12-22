namespace vg::gfx::vulkan
{
    //--------------------------------------------------------------------------------------
    Shader::Shader(const core::Blob & _bytecode) :
        super::Shader(_bytecode)
    {
        VkShaderModuleCreateInfo vkModuleDesc;

        vkModuleDesc.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vkModuleDesc.pNext = nullptr;
        vkModuleDesc.flags = 0;
        vkModuleDesc.codeSize = _bytecode.size();
        vkModuleDesc.pCode = (const u32*)_bytecode.data();

        VG_VERIFY_VULKAN(vkCreateShaderModule(gfx::Device::get()->getVulkanDevice(), &vkModuleDesc, nullptr, &m_vkShaderModule));
    }

    //--------------------------------------------------------------------------------------
    Shader::~Shader()
    {
        vkDestroyShaderModule(gfx::Device::get()->getVulkanDevice(), m_vkShaderModule, nullptr);
    }
}