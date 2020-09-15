namespace vg::graphics::driver::vulkan
{
    //--------------------------------------------------------------------------------------
    VkPolygonMode RasterizerState::getVulkanPolygonMode(FillMode _fillMode)
    {
        switch (_fillMode)
        {
        default:
            VG_ASSERT(false, "Unhandled FillMode \"%s\" (%u)", asString(_fillMode), _fillMode);
        case FillMode::Solid:
            return VK_POLYGON_MODE_FILL;

        case FillMode::Wireframe:
            return VK_POLYGON_MODE_LINE;
        }
    }

    //--------------------------------------------------------------------------------------
    VkCullModeFlagBits RasterizerState::getVulkanCullMode(CullMode _cullMode)
    {
        switch (_cullMode)
        {
        case CullMode::None:
            return VK_CULL_MODE_NONE;

        case CullMode::Back:
            return VK_CULL_MODE_BACK_BIT;

        case CullMode::Front:
            return VK_CULL_MODE_FRONT_BIT;
        }
    }

    //--------------------------------------------------------------------------------------
    VkPipelineRasterizationStateCreateInfo RasterizerState::getVulkanRasterizerState() const
    {
        VkPipelineRasterizationStateCreateInfo vkRs = {};

        vkRs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        vkRs.polygonMode = getVulkanPolygonMode(fillMode);
        vkRs.cullMode = getVulkanCullMode(cullMode);
        vkRs.frontFace = Orientation::ClockWise == orientation ? VK_FRONT_FACE_CLOCKWISE : VK_FRONT_FACE_COUNTER_CLOCKWISE; // Name is misleading, Vulkan uses different convention than DirectX and OpenGL
        vkRs.depthClampEnable = VK_FALSE;
        vkRs.rasterizerDiscardEnable = VK_FALSE;
        vkRs.depthBiasEnable = VK_FALSE;
        vkRs.lineWidth = 1.0f;

        return vkRs;
    }
}