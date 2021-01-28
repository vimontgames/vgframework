namespace vg::graphics::driver::dx12
{
    //--------------------------------------------------------------------------------------
    RootSignature::RootSignature(const RootSignatureDesc & _desc) :
        super::RootSignature(_desc)
    {
        vector<D3D12_ROOT_PARAMETER> d3d12rootParams;

        const auto & rootConstants = _desc.getRootConstants();
        for (uint i = 0; i < rootConstants.size(); ++i)
        {
            const RootSignatureDesc::PushConstantParams & rootConstant = rootConstants[i];

            D3D12_ROOT_PARAMETER d3d12RootConstants = {};
            d3d12RootConstants.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
            d3d12RootConstants.ShaderVisibility = getd3d12ShaderStageFlags(rootConstant.m_stages);
            d3d12RootConstants.Constants.ShaderRegister = rootConstant.m_start;
            d3d12RootConstants.Constants.Num32BitValues = rootConstant.m_count;
            d3d12RootConstants.Constants.RegisterSpace = rootConstant.m_space;

            d3d12rootParams.push_back(d3d12RootConstants);
        }

        const auto & tables = _desc.getTables();
        vector<vector<D3D12_DESCRIPTOR_RANGE>> d3d12DescriptorsArray;
        d3d12DescriptorsArray.resize(tables.size());

        for (uint i = 0; i < tables.size(); ++i)
        {
            const RootSignatureTableDesc & table = tables[i];
            const auto & descriptors = table.getDescriptors();
            
            auto & d3d12Descriptors = d3d12DescriptorsArray[i];
            for (uint j = 0; j < descriptors.size(); ++j)
            {
                const RootSignatureTableDesc::Descriptor & descriptor = descriptors[j];
            
                D3D12_DESCRIPTOR_RANGE d3d12Descriptor;

                switch (descriptor.getDescriptorType())
                {
                    default:
                    VG_ASSERT(false, "Unhandled Descriptor::Type \"%s\" (%u)", asString(descriptor.getDescriptorType()).c_str(), descriptor.getDescriptorType());
                    break;

                    case RootSignatureTableDesc::Descriptor::Type::ConstantBuffer:
                    {
                        const auto & constantbuffers = descriptor.getConstantBuffers();
                        
                        d3d12Descriptor.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
                        d3d12Descriptor.BaseShaderRegister = constantbuffers.m_offset;
                        d3d12Descriptor.NumDescriptors = constantbuffers.m_count;
                        d3d12Descriptor.RegisterSpace = constantbuffers.m_space;
                        d3d12Descriptor.OffsetInDescriptorsFromTableStart = constantbuffers.m_offset;

                        d3d12Descriptors.push_back(d3d12Descriptor);
                    }
                    break;

                    case RootSignatureTableDesc::Descriptor::Type::Texture:
                    {
                        const auto & textures = descriptor.getTextures();
                        
                        d3d12Descriptor.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                        d3d12Descriptor.BaseShaderRegister = textures.m_offset;
                        d3d12Descriptor.NumDescriptors = textures.m_count;
                        d3d12Descriptor.RegisterSpace = textures.m_space;
                        d3d12Descriptor.OffsetInDescriptorsFromTableStart = textures.m_offset;

                        d3d12Descriptors.push_back(d3d12Descriptor);
                    }
                    break;

                    case RootSignatureTableDesc::Descriptor::Type::Buffer:
                    {
                        const auto & buffers = descriptor.getBuffers();

                        d3d12Descriptor.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                        d3d12Descriptor.BaseShaderRegister = buffers.m_offset; 
                        d3d12Descriptor.NumDescriptors = buffers.m_count;
                        d3d12Descriptor.RegisterSpace = buffers.m_space;
                        d3d12Descriptor.OffsetInDescriptorsFromTableStart = buffers.m_offset;

                        d3d12Descriptors.push_back(d3d12Descriptor);
                    }
                    break;

                    case RootSignatureTableDesc::Descriptor::Type::UAVTexture:
                    {
                        const auto & textures = descriptor.getUAVTextures();

                        d3d12Descriptor.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
                        d3d12Descriptor.BaseShaderRegister = textures.m_offset;
                        d3d12Descriptor.NumDescriptors = textures.m_count;
                        d3d12Descriptor.RegisterSpace = textures.m_space;
                        d3d12Descriptor.OffsetInDescriptorsFromTableStart = textures.m_offset;

                        d3d12Descriptors.push_back(d3d12Descriptor);
                    }
                    break;

                    case RootSignatureTableDesc::Descriptor::Type::UAVBuffer:
                    {
                        const auto & buffers = descriptor.getUAVBuffers();

                        d3d12Descriptor.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
                        d3d12Descriptor.BaseShaderRegister = buffers.m_offset;
                        d3d12Descriptor.NumDescriptors = buffers.m_count;
                        d3d12Descriptor.RegisterSpace = buffers.m_space;
                        d3d12Descriptor.OffsetInDescriptorsFromTableStart = buffers.m_offset;

                        d3d12Descriptors.push_back(d3d12Descriptor);
                    }
                    break;
                }
            }
            
            D3D12_ROOT_PARAMETER d3d12Table = {};
            d3d12Table.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            d3d12Table.DescriptorTable.NumDescriptorRanges = (uint)d3d12Descriptors.size();
            d3d12Table.DescriptorTable.pDescriptorRanges = d3d12Descriptors.data();
            d3d12Table.ShaderVisibility = getd3d12ShaderStageFlags(table.getShaderStageFlags());

            d3d12rootParams.push_back(d3d12Table);
        }    

        // add static samplers
        vector<D3D12_STATIC_SAMPLER_DESC> d3d12SamplerStates;
        for (uint i = 0; i < enumCount<Sampler>(); ++i)
        {
            D3D12_STATIC_SAMPLER_DESC d3d12SamplerState = SamplerState::getd3d12SamplerState((Sampler)i);
            d3d12SamplerStates.push_back(d3d12SamplerState);
        }

        D3D12_ROOT_SIGNATURE_DESC d3d12Desc = {};
        d3d12Desc.NumParameters = (uint)d3d12rootParams.size();
        d3d12Desc.pParameters = d3d12rootParams.data();
        
        d3d12Desc.NumStaticSamplers = (uint)d3d12SamplerStates.size();
        d3d12Desc.pStaticSamplers = d3d12SamplerStates.data();

        ID3DBlob * outBlob = nullptr;
        ID3DBlob * errorBlob = nullptr;
        const HRESULT hr = D3D12SerializeRootSignature(&d3d12Desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &outBlob, &errorBlob);
        VG_ASSERT(SUCCEEDED(hr), "%s", errorBlob ? (char*)errorBlob->GetBufferPointer() : "");

        Device * device = driver::Device::get();
        VG_ASSERT_SUCCEEDED(device->getd3d12Device()->CreateRootSignature(0, outBlob->GetBufferPointer(), outBlob->GetBufferSize(), IID_PPV_ARGS(&m_d3d12rootSignature)));
    }

    //--------------------------------------------------------------------------------------
    RootSignature::~RootSignature()
    {
        VG_SAFE_RELEASE(m_d3d12rootSignature);
    }

    //--------------------------------------------------------------------------------------
    D3D12_SHADER_VISIBILITY RootSignature::getd3d12ShaderStageFlags(ShaderStageFlags _shaderStageFlags)
    {
        if (ShaderStageFlags::VS == _shaderStageFlags)
            return D3D12_SHADER_VISIBILITY_VERTEX;
        else if (ShaderStageFlags::HS == _shaderStageFlags)
            return D3D12_SHADER_VISIBILITY_HULL;
        else if (ShaderStageFlags::DS == _shaderStageFlags)
            return D3D12_SHADER_VISIBILITY_DOMAIN;
        else if (ShaderStageFlags::GS == _shaderStageFlags)
            return D3D12_SHADER_VISIBILITY_GEOMETRY;
        else if (ShaderStageFlags::PS == _shaderStageFlags)
            return D3D12_SHADER_VISIBILITY_PIXEL;
        else
            return D3D12_SHADER_VISIBILITY_ALL;
    }
}