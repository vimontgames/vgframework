namespace vg::gfx::vulkan
{ 
    //--------------------------------------------------------------------------------------
    VkBlendFactor BlendState::getVulkanBlendFactor(BlendFactor _blendFactor)
    {
        switch (_blendFactor)
        {
            default:
                VG_ASSERT(false, "Unhandled BlendFactor \"%s\" (%u)", asString(_blendFactor).c_str(), _blendFactor);

            case BlendFactor::Zero:
                return VK_BLEND_FACTOR_ZERO;

            case BlendFactor::One:
                return VK_BLEND_FACTOR_ONE;

            case BlendFactor::SrcAlpha:
                return VK_BLEND_FACTOR_SRC_ALPHA;

            case BlendFactor::OneMinusSrcAlpha:
                return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

            case BlendFactor::SrcColor:
                return VK_BLEND_FACTOR_SRC_COLOR;

            case BlendFactor::OneMinusSrcColor:
                return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        }
    }

    //--------------------------------------------------------------------------------------
    VkBlendOp BlendState::getVulkanBlendOp(BlendOp _blendOp)
    {
        switch (_blendOp)
        {
            default:
                VG_ASSERT(false, "Unhandled BlendFaBlendOpctor \"%s\" (%u)", asString(_blendOp).c_str(), _blendOp);

            case BlendOp::Add:
                return VK_BLEND_OP_ADD;
        }
    }

    //--------------------------------------------------------------------------------------
    VkPipelineColorBlendStateCreateInfo BlendState::getVulkanBlendState(VkPipelineColorBlendAttachmentState * _vkRenderTargetBlend, uint _renderTargetCount) const
    {
       static VkPipelineColorBlendAttachmentState rt[maxRenderTarget] = {};
       VkPipelineColorBlendStateCreateInfo bs = {};
       bs.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
       bs.attachmentCount = _renderTargetCount;
       bs.pAttachments = rt;

       for (uint i = 0; i < _renderTargetCount; ++i)
       {
           const auto & src = m_renderTargetBlend[i];
           auto & dst = rt[i];

           dst.colorWriteMask = 0;
           if (asBool(ColorWrite::Red & src.colorWrite))
               dst.colorWriteMask |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT;
           if (asBool(ColorWrite::Green & src.colorWrite))
               dst.colorWriteMask |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT;
           if (asBool(ColorWrite::Blue & src.colorWrite))
               dst.colorWriteMask |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT;
           if (asBool(ColorWrite::Alpha & src.colorWrite))
               dst.colorWriteMask |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT;

           dst.blendEnable = src.enable;

           dst.colorBlendOp = getVulkanBlendOp(src.blendOp);
           dst.srcColorBlendFactor = getVulkanBlendFactor(src.srcBlend);
           dst.dstColorBlendFactor = getVulkanBlendFactor(src.dstBlend);

           dst.alphaBlendOp = getVulkanBlendOp(src.blendOpAlpha);
           dst.srcAlphaBlendFactor = getVulkanBlendFactor(src.srcBlendAlpha);
           dst.dstAlphaBlendFactor = getVulkanBlendFactor(src.dstBlendAlpha);
       }

       return bs;
    }
}