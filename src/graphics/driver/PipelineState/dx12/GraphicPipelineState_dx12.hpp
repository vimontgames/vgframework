namespace vg::graphics::driver::dx12
{
    //--------------------------------------------------------------------------------------
    GraphicPipelineState::GraphicPipelineState(const GraphicPipelineStateKey & _key) :
        super::GraphicPipelineState(_key)
    {
        auto * device = driver::Device::get();
        auto * d3d12device = device->getd3d12Device();
        auto * sm = ShaderManager::get();

        D3D12_GRAPHICS_PIPELINE_STATE_DESC d3d12graphicPipelineDesc = {};

        VG_ASSERT(_key.m_rootSignature.isValid(), "Please set a valid RootSignature using CommandList::setRootSignature()");
        auto * sig = device->getRootSignature(_key.m_rootSignature);
        d3d12graphicPipelineDesc.pRootSignature = sig->getd3d12RootSignature();

        // temp
        auto & rt0 = d3d12graphicPipelineDesc.BlendState.RenderTarget[0];
        rt0.BlendEnable = false;
        rt0.LogicOpEnable = false;
        rt0.SrcBlend = D3D12_BLEND_ONE;
        rt0.DestBlend = D3D12_BLEND_ZERO;
        rt0.BlendOp = D3D12_BLEND_OP_ADD;
        rt0.SrcBlendAlpha = D3D12_BLEND_ONE;
        rt0.DestBlendAlpha = D3D12_BLEND_ZERO;
        rt0.BlendOpAlpha = D3D12_BLEND_OP_ADD;
        rt0.LogicOp = D3D12_LOGIC_OP_NOOP;
        rt0.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        
        //auto & ds = d3d12graphicPipelineDesc.DepthStencilState;
        //ds.DepthEnable = false;
        //ds.StencilWriteMask = 0;
        //ds.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        //ds.StencilEnable = false;
        //ds.StencilWriteMask = 0x0;
        //ds.StencilReadMask = 0xFF;
        //ds.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        //ds.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

        d3d12graphicPipelineDesc.SampleMask = 0xFFFFFFFF;

        auto * desc = sm->getHLSLDescriptor(_key.m_shaderKey.file);
        VG_ASSERT(desc);
        if (desc)
        {
            if (ShaderKey::VS(-1) != _key.m_shaderKey.vs)
                d3d12graphicPipelineDesc.VS = desc->getVS(_key.m_shaderKey.vs)->getd3d12Bytecode();

            if (ShaderKey::PS(-1) != _key.m_shaderKey.vs)
                d3d12graphicPipelineDesc.PS = desc->getPS(_key.m_shaderKey.ps)->getd3d12Bytecode();
        }

        d3d12graphicPipelineDesc.RasterizerState = _key.m_rasterizerState.getd3d12RasterizerState();
        d3d12graphicPipelineDesc.PrimitiveTopologyType = getd3d12PrimitiveTopologyType(_key.m_primitiveType);

        const auto subPassKey = _key.m_renderPassKey.m_subPassKeys[_key.m_subPassIndex];

        uint numRenderTargets = 0;
        for (uint i = 0; i < maxRenderTarget; ++i)
        {
            const auto flags = subPassKey.getRenderTargetFlags(i);
            if (SubPassKey::Flags::Bind & flags)
            {
                const auto format = _key.m_renderPassKey.m_colorFormat[i];
                VG_ASSERT(PixelFormat::Unknow != format);
                d3d12graphicPipelineDesc.RTVFormats[numRenderTargets] = Texture::getd3d12PixelFormat(format);
                ++numRenderTargets;
            }
            else
            {
                d3d12graphicPipelineDesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
            }
        }
        d3d12graphicPipelineDesc.NumRenderTargets = numRenderTargets;

        const auto flags = subPassKey.getDepthStencilFlags();
        if (SubPassKey::Flags::Bind & flags)
        {
            const auto format = _key.m_renderPassKey.m_depthStencilFormat;
            VG_ASSERT(PixelFormat::Unknow != format);
            d3d12graphicPipelineDesc.DSVFormat = Texture::getd3d12PixelFormat(format);
        }
        else
        {
            d3d12graphicPipelineDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
        }

        d3d12graphicPipelineDesc.SampleDesc.Count = 1;
        d3d12graphicPipelineDesc.SampleDesc.Quality = 0;

        VG_ASSERT_SUCCEEDED(d3d12device->CreateGraphicsPipelineState(&d3d12graphicPipelineDesc, IID_PPV_ARGS(&m_d3d12GraphicPipelineState)));
    }

    //--------------------------------------------------------------------------------------
    GraphicPipelineState::~GraphicPipelineState()
    {
        VG_SAFE_RELEASE(m_d3d12GraphicPipelineState);
    }

    //--------------------------------------------------------------------------------------
    D3D12_PRIMITIVE_TOPOLOGY_TYPE GraphicPipelineState::getd3d12PrimitiveTopologyType(PrimitiveType _primitiveType)
    {
        switch (_primitiveType)
        {
        case PrimitiveType::Point:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

        case PrimitiveType::Line:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

        default:
            VG_ASSERT(false, "Unhandled PrimitiveType \"%s\" (%u)", asString(_primitiveType), _primitiveType);
        case PrimitiveType::Triangle:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        case PrimitiveType::Patch:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
        }
    }
}