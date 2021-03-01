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

        d3d12graphicPipelineDesc.BlendState = _key.m_blendState.getd3d12BlendState();

        if (_key.m_depthStencilState.m_depthEnable)
        {
            d3d12graphicPipelineDesc.DepthStencilState.DepthEnable = true;
            d3d12graphicPipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
            d3d12graphicPipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
            d3d12graphicPipelineDesc.DepthStencilState.StencilEnable = false;
            d3d12graphicPipelineDesc.DepthStencilState.StencilReadMask = 0x00;
            d3d12graphicPipelineDesc.DepthStencilState.StencilWriteMask = 0x00;
            d3d12graphicPipelineDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
            d3d12graphicPipelineDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
            d3d12graphicPipelineDesc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
            d3d12graphicPipelineDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
            d3d12graphicPipelineDesc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
            d3d12graphicPipelineDesc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
            d3d12graphicPipelineDesc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
            d3d12graphicPipelineDesc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
        }
        else
        {
            d3d12graphicPipelineDesc.DepthStencilState.DepthEnable = false;
        }

        d3d12graphicPipelineDesc.SampleMask = 0xFFFFFFFF;

        auto * desc = sm->getHLSLDescriptor(_key.m_shaderKey.file);
        VG_ASSERT(desc);
        if (desc)
        {
            if (ShaderKey::VS(-1) != _key.m_shaderKey.vs)
                d3d12graphicPipelineDesc.VS = desc->getVS(API::DirectX12, _key.m_shaderKey.vs)->getd3d12Bytecode();

            if (ShaderKey::HS(-1) != _key.m_shaderKey.hs)
                d3d12graphicPipelineDesc.HS = desc->getHS(API::DirectX12, _key.m_shaderKey.hs)->getd3d12Bytecode();

            if (ShaderKey::DS(-1) != _key.m_shaderKey.ds)
                d3d12graphicPipelineDesc.DS = desc->getDS(API::DirectX12, _key.m_shaderKey.hs)->getd3d12Bytecode();

            if (ShaderKey::GS(-1) != _key.m_shaderKey.gs)
                d3d12graphicPipelineDesc.GS = desc->getGS(API::DirectX12, _key.m_shaderKey.hs)->getd3d12Bytecode();

            if (ShaderKey::PS(-1) != _key.m_shaderKey.vs)
                d3d12graphicPipelineDesc.PS = desc->getPS(API::DirectX12, _key.m_shaderKey.ps)->getd3d12Bytecode();
        }

        d3d12graphicPipelineDesc.RasterizerState = _key.m_rasterizerState.getd3d12RasterizerState();
        d3d12graphicPipelineDesc.PrimitiveTopologyType = getd3d12PrimitiveTopologyType(_key.getPrimitiveType());

        const auto subPassKey = _key.m_renderPassKey.m_subPassKeys[_key.m_subPassIndex];

        uint numRenderTargets = 0;
        for (uint i = 0; i < maxRenderTarget; ++i)
        {
            const SubPassKey::AttachmentInfo & info = subPassKey.getColorAttachmentInfo(i);
            if (asBool(SubPassKey::AttachmentFlags::RenderTarget & info.flags))
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

        const SubPassKey::AttachmentInfo info = subPassKey.getDepthStencilAttachmentInfo();
        if (asBool(SubPassKey::AttachmentFlags::RenderTarget & info.flags))
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
            default:
                VG_ASSERT(false, "Unhandled PrimitiveType \"%s\" (%u)", asString(_primitiveType).c_str(), _primitiveType);

            case PrimitiveType::Point:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

            case PrimitiveType::Line:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

            case PrimitiveType::Triangle:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

            //case PrimitiveType::Patch:
            //    return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
        }
    }
}