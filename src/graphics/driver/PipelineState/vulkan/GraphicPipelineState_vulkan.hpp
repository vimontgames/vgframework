namespace vg::graphics::driver::vulkan
{
    //--------------------------------------------------------------------------------------
    GraphicPipelineState::GraphicPipelineState(const GraphicPipelineStateKey & _key) :
        super::GraphicPipelineState(_key)
    {
        auto * device = driver::Device::get();
        auto & vkDevice = device->getVulkanDevice();

        uint renderTargetCount = 0;
        for (uint i = 0; i < maxRenderTarget; ++i)
        {
            const auto & passKey = _key.m_renderPassKey;
            const auto format = passKey.m_colorFormat[i];
            if (PixelFormat::Unknow != format)
                ++renderTargetCount;
        }            

        VkPipelineCacheCreateInfo vkPipelineCacheDesc = {};
        vkPipelineCacheDesc.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        VG_ASSERT_VULKAN(vkCreatePipelineCache(vkDevice, &vkPipelineCacheDesc, nullptr, &m_vkPipelineCache));

        VkGraphicsPipelineCreateInfo vkPipelineDesc = {};

        // Blend state
        VkPipelineColorBlendAttachmentState vkRenderTargets[maxRenderTarget];
        VkPipelineColorBlendStateCreateInfo bs = _key.m_blendState.getVulkanBlendState(&vkRenderTargets[0], renderTargetCount);
        vkPipelineDesc.pColorBlendState = &bs;

        // Vertex input
        VkPipelineVertexInputStateCreateInfo vi = {};
        vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vkPipelineDesc.pVertexInputState = &vi;

        // Input Assembly
        VkPipelineInputAssemblyStateCreateInfo ia = {};
        ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        ia.topology = getVulkanPrimitiveTopology(_key.m_primitiveTopology); 
        vkPipelineDesc.pInputAssemblyState = &ia;

        // PipelineLayout
        vkPipelineDesc.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        vkPipelineDesc.layout = device->getRootSignature(_key.m_rootSignature)->getVulkanPipelineLayout();

        // Dynamic states
        VkPipelineDynamicStateCreateInfo dynamicState = {};
        VkDynamicState dynamicStates[] =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };        
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.pDynamicStates = dynamicStates;
        dynamicState.dynamicStateCount = 2;
        vkPipelineDesc.pDynamicState = &dynamicState;

        // Viewport
        VkPipelineViewportStateCreateInfo vp = {};
        vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        vp.viewportCount = 1;
        vp.scissorCount = 1;
        vkPipelineDesc.pViewportState = &vp;

        // Rasterizer state
        VkPipelineRasterizationStateCreateInfo rs = _key.m_rasterizerState.getVulkanRasterizerState();
        vkPipelineDesc.pRasterizationState = &rs;

        // Multisample
        VkPipelineMultisampleStateCreateInfo ms = {};
        ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        ms.pSampleMask = nullptr;
        ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        vkPipelineDesc.pMultisampleState = &ms;

        // Shaders
        vector<VkPipelineShaderStageCreateInfo> vkShaderStages;

        auto * sm = ShaderManager::get();
        auto & hlslDesc = *sm->getHLSLDescriptor(_key.m_shaderKey.file);

        if (ShaderKey::VS(-1) != _key.m_shaderKey.vs)
            addVulkanShader(vkShaderStages, hlslDesc, ShaderStage::Vertex, _key.m_shaderKey.vs);

        if (ShaderKey::PS(-1) != _key.m_shaderKey.ps)
            addVulkanShader(vkShaderStages, hlslDesc, ShaderStage::Pixel, _key.m_shaderKey.ps);

        vkPipelineDesc.stageCount = (uint)vkShaderStages.size();
        vkPipelineDesc.pStages = vkShaderStages.data();

        // Get the exact same render pass using _key.m_renderPassKey
        vkPipelineDesc.renderPass = device->getVulkanRenderPass(_key.m_renderPassKey);
        vkPipelineDesc.subpass = _key.m_subPassIndex;

        VG_ASSERT_VULKAN(vkCreateGraphicsPipelines(vkDevice, m_vkPipelineCache, 1, &vkPipelineDesc, nullptr, &m_vkPipeline));
    }

    //--------------------------------------------------------------------------------------
    GraphicPipelineState::~GraphicPipelineState()
    {
        auto & vkDevice = driver::Device::get()->getVulkanDevice();

        vkDestroyPipeline(vkDevice, m_vkPipeline, nullptr);
        vkDestroyPipelineCache(vkDevice, m_vkPipelineCache, nullptr);
    }

    //--------------------------------------------------------------------------------------
    VkShaderStageFlagBits GraphicPipelineState::getVulkanShaderStage(ShaderStage _stage)
    {
        switch (_stage)
        {
        default:
            VG_ASSERT(false, "Unhandled ShaderStage \"%s\" (%u)", asString(_stage).c_str(), _stage);
            return VkShaderStageFlagBits(0);

        case ShaderStage::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;

        case ShaderStage::Hull:
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

        case ShaderStage::Domain:
            return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

        case ShaderStage::Geometry:
            return VK_SHADER_STAGE_GEOMETRY_BIT;

        case ShaderStage::Pixel:
            return VK_SHADER_STAGE_FRAGMENT_BIT;

        case ShaderStage::Compute:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        }
    }

    //--------------------------------------------------------------------------------------
    VkPrimitiveTopology GraphicPipelineState::getVulkanPrimitiveTopology(PrimitiveTopology _topology)
    {
        switch (_topology)
        {
            default:
                VG_ASSERT(false, "Unhandled PrimitiveTopology \"%s\" (%u)", asString(_topology).c_str(), _topology);

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
    void GraphicPipelineState::addVulkanShader(vector<VkPipelineShaderStageCreateInfo> & _vkStages, HLSLDesc & _hlsl, ShaderStage _stage, uint _index)
    {
        const Shader * shader = _hlsl.getShader(API::Vulkan, _stage, _index);

        VkPipelineShaderStageCreateInfo vkShaderStage = {};

        vkShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vkShaderStage.stage = getVulkanShaderStage(_stage);
        vkShaderStage.module = shader->getVulkanBytecode();
        vkShaderStage.pName = shader->getName().c_str();

        _vkStages.push_back(vkShaderStage);
    }
}