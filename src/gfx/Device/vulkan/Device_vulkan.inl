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

    //--------------------------------------------------------------------------------------
    VG_INLINE VkInstance & Device::getVulkanInstance()
    {
        return m_vkInstance;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE VkDevice & Device::getVulkanDevice()
    {
        return m_vkDevice;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE VkPhysicalDevice & Device::getVulkanPhysicalDevice()
    {
        return m_vkPhysicalDevice;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE VkPhysicalDeviceProperties & Device::getVulkanPhysicalDeviceProperties()
    {
        return m_vkPhysicalDeviceProperties;
    }
}