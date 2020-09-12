namespace vg::graphics::driver::dx12
{
    //--------------------------------------------------------------------------------------
    D3D12_FILL_MODE RasterizerState::getd3d12FillMode(FillMode _fillMode)
    {
        switch (_fillMode)
        {
        default:
            VG_ASSERT(false, "Unhandled FillMode \"%s\" (%u)", asString(_fillMode), _fillMode);
        case RasterizerState::FillMode::Solid:
            return D3D12_FILL_MODE_SOLID;

        case RasterizerState::FillMode::Wireframe:
            return D3D12_FILL_MODE_WIREFRAME;
        }
    }

    //--------------------------------------------------------------------------------------
    D3D12_CULL_MODE RasterizerState::getd3d12CullMode(CullMode _cullMode)
    {
        switch (_cullMode)
        {
        default:
            VG_ASSERT(false, "Unhandled CullMode \"%s\" (%u)", asString(_cullMode), _cullMode);
        case RasterizerState::CullMode::None:
            return D3D12_CULL_MODE_NONE;

        case RasterizerState::CullMode::Back:
            return D3D12_CULL_MODE_BACK;

        case RasterizerState::CullMode::Front:
            return D3D12_CULL_MODE_FRONT;
        }
    }

    //--------------------------------------------------------------------------------------
    D3D12_RASTERIZER_DESC RasterizerState::getd3d12RasterizerState() const
    {
        D3D12_RASTERIZER_DESC d3d12desc = {};
        
        d3d12desc.CullMode = getd3d12CullMode(m_cullMode);
        d3d12desc.FillMode = getd3d12FillMode(m_fillMode);

        return d3d12desc;
    }
}