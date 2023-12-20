namespace vg::gfx::vulkan
{
    //--------------------------------------------------------------------------------------
    VG_INLINE VkDescriptorSet Device::getVulkanBindlessDescriptors() const 
    { 
        return m_vkBindlessDescriptors; 
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE VkSwapchainKHR Device::getVulkanSwapchain() const
    { 
        return m_vkSwapchain; 
    }
}