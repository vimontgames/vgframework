namespace vg::gfx::vulkan
{
    //--------------------------------------------------------------------------------------
    VG_INLINE void ComputePipelineState::setVulkanComputePipelineState(const VkPipelineCache & _vkPipelineCache, const VkPipeline & _vkPipeline)
    {
        m_vkComputePipelineCache = _vkPipelineCache;
        m_vkComputePipeline = _vkPipeline;
    }
}

