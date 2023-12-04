namespace vg::gfx::dx12
{
    //--------------------------------------------------------------------------------------
    D3D12_FILL_MODE RasterizerState::getd3d12FillMode(FillMode _fillMode)
    {
        switch (_fillMode)
        {
        default:
            VG_ASSERT(false, "Unhandled FillMode \"%s\" (%u)", asString(_fillMode).c_str(), _fillMode);
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
            VG_ASSERT(false, "Unhandled CullMode \"%s\" (%u)", asString(_cullMode).c_str(), _cullMode);
        case CullMode::None:
            return D3D12_CULL_MODE_NONE;

        case CullMode::Back:
            return D3D12_CULL_MODE_BACK;

        case CullMode::Front:
            return D3D12_CULL_MODE_FRONT;
        }
    }

    //--------------------------------------------------------------------------------------
    // There are two options for calculating depth bias.
    // 
    // If the depth buffer currently bound to the output - merger stage has a UNORM format or no depth buffer is bound, the bias value is calculated like this:
    //      Bias = (float)DepthBias * r + SlopeScaledDepthBias * MaxDepthSlope;
    // where r is the minimum representable value > 0 in the depth - buffer format converted to float32.The DepthBias and SlopeScaledDepthBias values are D3D11_RASTERIZER_DESC1 structure members.The MaxDepthSlope value is the maximum of the horizontal and vertical slopes of the depth value at the pixel.
    //
    // If a floating - point depth buffer is bound to the output - merger stage the bias value is calculated like this:
    //      Bias = (float)DepthBias * 2^((max z in primitive) - r) + SlopeScaledDepthBias * MaxDepthSlope;
    // where r is the number of mantissa bits in the floating point representation(excluding the hidden bit); for example, 23 for float32.
    // 
    // https://learn.microsoft.com/en-us/windows/win32/direct3d11/d3d10-graphics-programming-guide-output-merger-stage-depth-bias
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
            VG_ASSERT(false, "Unhandled DepthBias \"%s\" (%u)", asString(depthBias).c_str(), depthBias);
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
        //    d3d12desc.DepthBias = 200;
        //    d3d12desc.DepthBiasClamp = 1.0f;
        //    d3d12desc.SlopeScaledDepthBias = 1.0f;
        //    break;
        }

        return d3d12desc;
    }
}