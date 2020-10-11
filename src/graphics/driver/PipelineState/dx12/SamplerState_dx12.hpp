namespace vg::graphics::driver::dx12
{
    //--------------------------------------------------------------------------------------
    D3D12_STATIC_SAMPLER_DESC SamplerState::getd3d12SamplerState(Sampler _sampler)
    {
        const driver::SamplerState samplerState(_sampler);

        D3D12_STATIC_SAMPLER_DESC desc = {};

        desc.Filter = getd3d12Filter(samplerState.filter);

        desc.AddressU = getd3d12Adress(samplerState.address);
        desc.AddressV = getd3d12Adress(samplerState.address);
        desc.AddressW = getd3d12Adress(samplerState.address);

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