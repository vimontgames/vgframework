namespace vg::graphics::driver::dx12
{
    //--------------------------------------------------------------------------------------
    D3D12_FILL_MODE RasterizerState::getd3d12FillMode(FillMode _fillMode)
    {
        switch (_fillMode)
        {
        default:
            VG_ASSERT(false, "Unhandled FillMode \"%s\" (%u)", asString(_fillMode), _fillMode);
        case FillMode::Solid:
            return D3D12_FILL_MODE_SOLID;

        case FillMode::Wireframe:
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
        case CullMode::None:
            return D3D12_CULL_MODE_NONE;

        case CullMode::Back:
            return D3D12_CULL_MODE_BACK;

        case CullMode::Front:
            return D3D12_CULL_MODE_FRONT;
        }
    }

    //--------------------------------------------------------------------------------------
    D3D12_RASTERIZER_DESC RasterizerState::getd3d12RasterizerState() const
    {
        D3D12_RASTERIZER_DESC d3d12desc = {};
        
        d3d12desc.CullMode = getd3d12CullMode(cullMode);
        d3d12desc.FillMode = getd3d12FillMode(fillMode);

        d3d12desc.DepthClipEnable = DepthClip::Enable == depthClip ? true : false;

        d3d12desc.FrontCounterClockwise = Orientation::CounterClockWise == orientation ? true : false;
        
        switch (depthBias)
        {
        default:
            VG_ASSERT(false, "Unhandled DepthBias \"%s\" (%u)", asString(depthBias), depthBias);
        case DepthBias::None:
            d3d12desc.DepthBias = 0;
            d3d12desc.DepthBiasClamp = 0.0f;
            d3d12desc.SlopeScaledDepthBias = 0.0f;
            break;

        //case DepthBias::Small:
        //    d3d12desc.DepthBias = 0;
        //    d3d12desc.DepthBiasClamp = 0.0f;
        //    d3d12desc.SlopeScaledDepthBias = 0.0f;
        //    break;
        //
        //case DepthBias::Medium:
        //    d3d12desc.DepthBias = 0;
        //    d3d12desc.DepthBiasClamp = 0.0f;
        //    d3d12desc.SlopeScaledDepthBias = 0.0f;
        //    break;
        //
        //case DepthBias::Big:
        //    d3d12desc.DepthBias = 0;
        //    d3d12desc.DepthBiasClamp = 0.0f;
        //    d3d12desc.SlopeScaledDepthBias = 0.0f;
        //    break;
        }

        return d3d12desc;
    }
}