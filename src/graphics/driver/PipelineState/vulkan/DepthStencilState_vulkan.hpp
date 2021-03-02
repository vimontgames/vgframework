namespace vg::graphics::driver::vulkan
{
    //--------------------------------------------------------------------------------------
    VkCompareOp DepthStencilState::getVulkanCompareFunc(ComparisonFunc _cmpFunc)
    {
        switch (_cmpFunc)
        {
            default:
                VG_ASSERT(false, "Unhandled ComparisonFunc \"%s\" (%u)", asString(_cmpFunc).c_str(), _cmpFunc);

            case ComparisonFunc::Always:
                return VK_COMPARE_OP_ALWAYS;
            case ComparisonFunc::Never:
                return VK_COMPARE_OP_NEVER;
            case ComparisonFunc::Equal:
                return VK_COMPARE_OP_EQUAL;
            case ComparisonFunc::NotEqual:
                return VK_COMPARE_OP_NOT_EQUAL;
            case ComparisonFunc::Less:
                return VK_COMPARE_OP_LESS;
            case ComparisonFunc::LessEqual:
                return VK_COMPARE_OP_LESS_OR_EQUAL;
            case ComparisonFunc::Greater:
                return VK_COMPARE_OP_GREATER;
            case ComparisonFunc::GreaterEqual:
                return VK_COMPARE_OP_GREATER_OR_EQUAL;
        }
    }

    //--------------------------------------------------------------------------------------
    VkStencilOp DepthStencilState::getVulkanStencilOp(StencilOp _stencilOp)
    {
        switch (_stencilOp)
        {
            default:
                VG_ASSERT(false, "Unhandled StencilOp \"%s\" (%u)", asString(_stencilOp).c_str(), _stencilOp);

            case StencilOp::Zero:
                return VK_STENCIL_OP_ZERO;
            case StencilOp::Keep:
                return VK_STENCIL_OP_KEEP;
            case StencilOp::Replace:
                return VK_STENCIL_OP_REPLACE;
            case StencilOp::IncrementSat:
                return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
            case StencilOp::DecrementSat:
                return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
            case StencilOp::Invert:
                return VK_STENCIL_OP_INVERT;
            case StencilOp::IncrementWrap:
                return VK_STENCIL_OP_INCREMENT_AND_WRAP;
            case StencilOp::DecrementWrap:
                return VK_STENCIL_OP_DECREMENT_AND_WRAP;
        }
    }

    //--------------------------------------------------------------------------------------
    VkPipelineDepthStencilStateCreateInfo DepthStencilState::getVulkanDepthStencilState() const
    {
        VkPipelineDepthStencilStateCreateInfo vkDepthStencilState;

        vkDepthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        vkDepthStencilState.pNext = nullptr;

        vkDepthStencilState.depthTestEnable = m_depthEnable;
        vkDepthStencilState.depthWriteEnable = m_depthWrite;
        vkDepthStencilState.flags = 0x0;
        vkDepthStencilState.depthCompareOp = getVulkanCompareFunc(m_depthFunc);
        vkDepthStencilState.depthBoundsTestEnable = false;
        vkDepthStencilState.minDepthBounds = 0.0f;
        vkDepthStencilState.maxDepthBounds = 1.0f;

        vkDepthStencilState.stencilTestEnable = m_stencilEnable;

        vkDepthStencilState.front.compareOp = getVulkanCompareFunc(m_stencilFront.func);
        vkDepthStencilState.front.passOp = getVulkanStencilOp(m_stencilFront.passOp);
        vkDepthStencilState.front.failOp = getVulkanStencilOp(m_stencilFront.failOp);
        vkDepthStencilState.front.depthFailOp = getVulkanStencilOp(m_stencilFront.depthFailOp);
        vkDepthStencilState.front.reference = 0x0;
        vkDepthStencilState.front.compareMask = m_stencilReadMask;
        vkDepthStencilState.front.writeMask = m_stencilWriteMask;

        vkDepthStencilState.back.compareOp = getVulkanCompareFunc(m_stencilBack.func);
        vkDepthStencilState.back.passOp = getVulkanStencilOp(m_stencilBack.passOp);
        vkDepthStencilState.back.failOp = getVulkanStencilOp(m_stencilBack.failOp);
        vkDepthStencilState.back.depthFailOp = getVulkanStencilOp(m_stencilBack.depthFailOp);
        vkDepthStencilState.back.reference = 0x0;
        vkDepthStencilState.back.compareMask = m_stencilReadMask;
        vkDepthStencilState.back.writeMask = m_stencilWriteMask;  

        return vkDepthStencilState;
    }
}