#pragma once

#include "gfx/Shader/Shader.h"

namespace vg::gfx::vulkan
{
    class Shader : public base::Shader
    {
        using super = base::Shader;

    public:
        Shader(const core::Blob & _bytecode);
        ~Shader();

        const VkShaderModule & getVulkanBytecode() const { return m_vkShaderModule; }

    private:
        VkShaderModule   m_vkShaderModule;
    }; 
}