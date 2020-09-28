namespace vg::graphics::driver::dx12
{
    //--------------------------------------------------------------------------------------
    BindlessTable::BindlessTable() :
        super::BindlessTable()
    {
        auto * device = driver::Device::get();
        auto * d3d12device = device->getd3d12Device();

        // SRV/CBV/UAV CPU descriptor heap
        D3D12_DESCRIPTOR_HEAP_DESC srvCPUHeapDesc = {};
        srvCPUHeapDesc.NumDescriptors = max_bindless_elements;
        srvCPUHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvCPUHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        VG_ASSERT_SUCCEEDED(d3d12device->CreateDescriptorHeap(&srvCPUHeapDesc, IID_PPV_ARGS(&m_srvCPUDescriptorHeap)));
        m_srcDescriptorHeapSize = d3d12device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        // SRV/CBV/UAV GPU descriptor heap
        D3D12_DESCRIPTOR_HEAP_DESC srvGPUHeapDesc = srvCPUHeapDesc;
        srvGPUHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        VG_ASSERT_SUCCEEDED(d3d12device->CreateDescriptorHeap(&srvGPUHeapDesc, IID_PPV_ARGS(&m_srvGPUDescriptorHeap)));
    }

    //--------------------------------------------------------------------------------------
    BindlessTable::~BindlessTable()
    {
        auto * device = driver::Device::get();
        auto * d3d12device = device->getd3d12Device();

        VG_SAFE_RELEASE(m_srvCPUDescriptorHeap);
        VG_SAFE_RELEASE(m_srvGPUDescriptorHeap);
    }

    //--------------------------------------------------------------------------------------
    void BindlessTable::beginFrame()
    {
        D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptors = m_srvCPUDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        D3D12_CPU_DESCRIPTOR_HANDLE gpuDescriptors = m_srvGPUDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        auto * device = driver::Device::get();
        device->getd3d12Device()->CopyDescriptorsSimple(max_bindless_elements, gpuDescriptors, cpuDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    //--------------------------------------------------------------------------------------
    D3D12_CPU_DESCRIPTOR_HANDLE BindlessTable::getd3d12DescriptorHandle(BindlessHandle _handle) const
    {
        D3D12_CPU_DESCRIPTOR_HANDLE handle = { m_srvCPUDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
        handle.ptr += _handle * m_srcDescriptorHeapSize;
        return handle;
    }

    //--------------------------------------------------------------------------------------
    void BindlessTable::copyTextureHandle(core::uint _slot, driver::Texture * _texture)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE src = getd3d12DescriptorHandle(_texture->getBindlessTextureHandle());
        D3D12_CPU_DESCRIPTOR_HANDLE dst = getd3d12DescriptorHandle(_slot);

        auto * device = driver::Device::get();
        device->getd3d12Device()->CopyDescriptorsSimple(1, dst, src, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
}