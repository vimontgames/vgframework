namespace vg::gfx::dx12
{
    //--------------------------------------------------------------------------------------
    D3D12_STATIC_SAMPLER_DESC SamplerState::getd3d12SamplerState(Sampler _sampler)
    {
        const gfx::SamplerState samplerState(_sampler);

        D3D12_STATIC_SAMPLER_DESC desc = {};

        desc.Filter = getd3d12Filter(samplerState.filter);

        desc.AddressU = getd3d12Adress(samplerState.address);
        desc.AddressV = getd3d12Adress(samplerState.address);
        desc.AddressW = getd3d12Adress(samplerState.address);

        switch (samplerState.anisotropy)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(samplerState.anisotropy);

            case Anisotropy::Anisotropy_None:
                desc.MaxAnisotropy = 1;
                break;

            case Anisotropy::Anisotropy_2X:
                desc.MaxAnisotropy = 2;
                desc.Filter = D3D12_FILTER_ANISOTROPIC;
                break;

            case Anisotropy::Anisotropy_4X:
                desc.MaxAnisotropy = 4;
                desc.Filter = D3D12_FILTER_ANISOTROPIC;
                break;

            case Anisotropy::Anisotropy_8X:
                desc.MaxAnisotropy = 8;
                desc.Filter = D3D12_FILTER_ANISOTROPIC;
                break;

            case Anisotropy::Anisotropy_16X:
                desc.MaxAnisotropy = 16;
                desc.Filter = D3D12_FILTER_ANISOTROPIC;
                break;
        }

        desc.MipLODBias = 0; 
        desc.MinLOD = 0;
        desc.MaxLOD = D3D12_FLOAT32_MAX;        

        if (samplerState.filter == Filter::DepthCmp)
        {
            #if VG_GFX_REVERSE_DEPTH
            desc.ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
            #else
            desc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
            #endif
        }
        else
        {
            desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        }
        
        desc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;

        desc.ShaderRegister = (uint)_sampler;
        desc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        return desc;
    }

    //--------------------------------------------------------------------------------------
    D3D12_FILTER SamplerState::getd3d12Filter(Filter _filter)
    {
        switch (_filter)
        {
            default:
                VG_ASSERT(false, "Unhandled Filter \"%s\" (%u)", asString(_filter).c_str(), _filter);
            case Filter::Nearest:
                return D3D12_FILTER_MIN_MAG_MIP_POINT;
            case Filter::Linear:
                return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
            case Filter::DepthCmp:
                return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        }
    }

    //--------------------------------------------------------------------------------------
    D3D12_TEXTURE_ADDRESS_MODE SamplerState::getd3d12Adress(Address _address)
    {
        switch (_address)
        {
            default:
                VG_ASSERT(false, "Unhandled Address \"%s\" (%u)", asString(_address).c_str(), _address);
            case Address::Repeat:
                return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            case Address::Clamp:
                return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
            case Address::Mirror:
                return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        }
    }
}