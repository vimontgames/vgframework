namespace vg::graphics::driver::dx12
{
    //--------------------------------------------------------------------------------------
    RootSignature::RootSignature(const RootSignatureDesc & _desc) :
        super::RootSignature(_desc)
    {
        D3D12_ROOT_SIGNATURE_DESC d3d12Desc = {};

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
}