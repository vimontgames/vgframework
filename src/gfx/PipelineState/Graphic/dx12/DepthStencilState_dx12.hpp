namespace vg::gfx::dx12
{
    //--------------------------------------------------------------------------------------
    D3D12_COMPARISON_FUNC DepthStencilState::getd3d12ComparisonFunc(ComparisonFunc _cmpFunc)
    {
        switch (_cmpFunc)
        {
            default:
                VG_ASSERT(false, "Unhandled ComparisonFunc \"%s\" (%u)", asString(_cmpFunc).c_str(), _cmpFunc);

            case ComparisonFunc::Always:
                return D3D12_COMPARISON_FUNC_ALWAYS;
            case ComparisonFunc::Never:
                return D3D12_COMPARISON_FUNC_NEVER;
            case ComparisonFunc::Equal:
                return D3D12_COMPARISON_FUNC_EQUAL;
            case ComparisonFunc::NotEqual:
                return D3D12_COMPARISON_FUNC_NOT_EQUAL;

            #if VG_GFX_REVERSE_DEPTH
            case ComparisonFunc::Less:
                return D3D12_COMPARISON_FUNC_GREATER ;
            case ComparisonFunc::LessEqual:
                return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
            case ComparisonFunc::Greater:
                return D3D12_COMPARISON_FUNC_LESS;
            case ComparisonFunc::GreaterEqual:
                return D3D12_COMPARISON_FUNC_LESS_EQUAL ;
            #else
            case ComparisonFunc::Less:
                return D3D12_COMPARISON_FUNC_LESS;
            case ComparisonFunc::LessEqual:
                return D3D12_COMPARISON_FUNC_LESS_EQUAL;
            case ComparisonFunc::Greater:
                return D3D12_COMPARISON_FUNC_GREATER;
            case ComparisonFunc::GreaterEqual:
                return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
            #endif
        }
    }

    //--------------------------------------------------------------------------------------
    D3D12_STENCIL_OP DepthStencilState::getd3d12StencilOp(StencilOp _stencilOp)
    {
        switch (_stencilOp)
        {
            default:
                VG_ASSERT(false, "Unhandled StencilOp \"%s\" (%u)", asString(_stencilOp).c_str(), _stencilOp);

            case StencilOp::Zero:
                return D3D12_STENCIL_OP_ZERO;
            case StencilOp::Keep:
                return D3D12_STENCIL_OP_KEEP;
            case StencilOp::Replace:
                return D3D12_STENCIL_OP_REPLACE;
            case StencilOp::IncrementSat:
                return D3D12_STENCIL_OP_INCR_SAT;
            case StencilOp::DecrementSat:
                return D3D12_STENCIL_OP_DECR_SAT;
            case StencilOp::Invert:
                return D3D12_STENCIL_OP_INVERT;
            case StencilOp::IncrementWrap:
                return D3D12_STENCIL_OP_INCR;
            case StencilOp::DecrementWrap:
                return D3D12_STENCIL_OP_DECR;
        }
    }

    //--------------------------------------------------------------------------------------
    D3D12_DEPTH_STENCIL_DESC DepthStencilState::getd3d12DepthStencilState() const
    {
        D3D12_DEPTH_STENCIL_DESC d3d12Desc;

        d3d12Desc.DepthEnable = m_depthEnable;
        d3d12Desc.DepthWriteMask = m_depthWrite ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
        d3d12Desc.DepthFunc = getd3d12ComparisonFunc(m_depthFunc);
           
        d3d12Desc.StencilEnable = m_stencilEnable;
        d3d12Desc.StencilReadMask = m_stencilReadMask;
        d3d12Desc.StencilWriteMask = m_stencilWriteMask;

        d3d12Desc.FrontFace.StencilFunc = getd3d12ComparisonFunc(m_stencilFront.func);
        d3d12Desc.FrontFace.StencilPassOp = getd3d12StencilOp(m_stencilFront.passOp);
        d3d12Desc.FrontFace.StencilFailOp = getd3d12StencilOp(m_stencilFront.failOp);
        d3d12Desc.FrontFace.StencilDepthFailOp = getd3d12StencilOp(m_stencilFront.depthFailOp);

        d3d12Desc.BackFace.StencilFunc = getd3d12ComparisonFunc(m_stencilBack.func);
        d3d12Desc.BackFace.StencilPassOp = getd3d12StencilOp(m_stencilBack.passOp);
        d3d12Desc.BackFace.StencilFailOp = getd3d12StencilOp(m_stencilBack.failOp);
        d3d12Desc.BackFace.StencilDepthFailOp = getd3d12StencilOp(m_stencilBack.depthFailOp);

        return d3d12Desc;
    }
}