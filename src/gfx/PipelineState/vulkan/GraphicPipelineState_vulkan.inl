namespace vg::gfx::vulkan
{
    //--------------------------------------------------------------------------------------
    VG_INLINE VkPrimitiveTopology GraphicPipelineState::getVulkanPrimitiveTopology(PrimitiveTopology _topology)
    {
        switch (_topology)
        {
            default:
                VG_ASSERT(false, "Unhandled PrimitiveTopology \"%s\" (%u)", core::asString(_topology).c_str(), _topology);

            case PrimitiveTopology::PointList:
                return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

            case PrimitiveTopology::LineList:
                return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

            case PrimitiveTopology::LineStrip:
                return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;

            case PrimitiveTopology::TriangleList:
                return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

            case PrimitiveTopology::TriangleStrip:
                return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        }
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void GraphicPipelineState::setVulkanGraphicPipelineState(const VkPipelineCache & _vkPipelineCache, const VkPipeline & _vkPipeline)
    {
        m_vkPipelineCache = _vkPipelineCache;
        m_vkPipeline = _vkPipeline;
    }
}

