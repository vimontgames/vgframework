namespace vg::gfx::dx12
{
    //--------------------------------------------------------------------------------------
    D3D12_BLEND BlendState::getd3d12Blend(BlendFactor _blend)
    {
        switch (_blend)
        {
            default:
                VG_ASSERT(false, "Unhandled BlendFactor \"%s\" (%u)", asString(_blend).c_str(), _blend);

            case BlendFactor::Zero:
                return D3D12_BLEND_ZERO;

            case BlendFactor::One:
                return D3D12_BLEND_ONE;

            case BlendFactor::SrcAlpha:
                return D3D12_BLEND_SRC_ALPHA;

            case BlendFactor::OneMinusSrcAlpha:
                return D3D12_BLEND_INV_SRC_ALPHA;

            case BlendFactor::SrcColor:
                return D3D12_BLEND_SRC_COLOR;

            case BlendFactor::OneMinusSrcColor:
                return D3D12_BLEND_INV_SRC_COLOR;
        }
    }

    //--------------------------------------------------------------------------------------
    D3D12_BLEND_OP BlendState::getd3d12BlendOp(BlendOp _blendOp)
    {
        switch (_blendOp)
        {
            default:
                VG_ASSERT(false, "Unhandled BlendFaBlendOpctor \"%s\" (%u)", asString(_blendOp).c_str(), _blendOp);

            case BlendOp::Add:
                return D3D12_BLEND_OP_ADD;
        }
    }

    //--------------------------------------------------------------------------------------
    D3D12_BLEND_DESC BlendState::getd3d12BlendState() const
    {
        D3D12_BLEND_DESC d3d12desc = {};

        d3d12desc.AlphaToCoverageEnable = asBool(m_flags & BlendStateFlags::AlphaToCoverage) ? true : false;
        d3d12desc.IndependentBlendEnable = asBool(m_flags & BlendStateFlags::IndependantBlend) ? true : false;

        for (uint i = 0; i < maxRenderTarget; ++i)
        {
            const RenderTargetBlend & src = m_renderTargetBlend[i];
            D3D12_RENDER_TARGET_BLEND_DESC & dst = d3d12desc.RenderTarget[i];

            dst.BlendEnable = src.enable;
            dst.LogicOpEnable = false;
            dst.SrcBlend = getd3d12Blend(src.srcBlend);
            dst.DestBlend = getd3d12Blend(src.dstBlend);
            dst.BlendOp = getd3d12BlendOp(src.blendOp); 
            dst.SrcBlendAlpha = getd3d12Blend(src.srcBlendAlpha);
            dst.DestBlendAlpha = getd3d12Blend(src.dstBlendAlpha);
            dst.BlendOpAlpha = getd3d12BlendOp(src.blendOpAlpha);
            dst.LogicOp = D3D12_LOGIC_OP_NOOP;
            dst.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        }

        return d3d12desc;
    }
}