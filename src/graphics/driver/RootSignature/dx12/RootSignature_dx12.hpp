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
        for (uint i = 0; i < tables.size(); ++i)
        {
            const RootSignatureDesc::Table & table = tables[i];
            const auto & descriptors = table.getDescriptors();
            
            core::vector<D3D12_DESCRIPTOR_RANGE> d3d12Descriptors;
            for (uint j = 0; j < descriptors.size(); ++j)
            {
                const RootSignatureDesc::Table::Descriptor & descriptor = descriptors[j];
            
                D3D12_DESCRIPTOR_RANGE d3d12Descriptor;

                switch (descriptor.getDescriptorType())
                {
                    case RootSignatureDesc::Table::Descriptor::Type::Texture:
                    {
                        const auto & textures = descriptor.getTextures();
                        
                        d3d12Descriptor.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                        d3d12Descriptor.BaseShaderRegister = textures.m_register;
                        d3d12Descriptor.NumDescriptors = textures.m_count;
                        d3d12Descriptor.RegisterSpace = textures.m_space;
                        d3d12Descriptor.OffsetInDescriptorsFromTableStart = textures.m_offset;

                        d3d12Descriptors.push_back(d3d12Descriptor);
                    }
                    break;
                }
            
            }
            
            D3D12_ROOT_PARAMETER d3d12Table = {};
            d3d12Table.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            d3d12Table.DescriptorTable.NumDescriptorRanges = (uint)d3d12Descriptors.size();
            d3d12Table.DescriptorTable.pDescriptorRanges = d3d12Descriptors.data();
        }       

        D3D12_ROOT_SIGNATURE_DESC d3d12Desc = {};
        d3d12Desc.NumParameters = (uint)d3d12rootParams.size();
        d3d12Desc.pParameters = d3d12rootParams.data();

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
        if (testAllFlags(_shaderStageFlags, ShaderStageFlags::VS))
            return D3D12_SHADER_VISIBILITY_VERTEX;
        else if (testAllFlags(_shaderStageFlags, ShaderStageFlags::HS))
            return D3D12_SHADER_VISIBILITY_HULL;
        else if (testAllFlags(_shaderStageFlags, ShaderStageFlags::DS))
            return D3D12_SHADER_VISIBILITY_DOMAIN;
        else if (testAllFlags(_shaderStageFlags, ShaderStageFlags::GS))
            return D3D12_SHADER_VISIBILITY_GEOMETRY;
        else if (testAllFlags(_shaderStageFlags, ShaderStageFlags::PS))
            return D3D12_SHADER_VISIBILITY_PIXEL;
        else
            return D3D12_SHADER_VISIBILITY_ALL;
    }
}