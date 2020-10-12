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
        srvCPUHeapDesc.NumDescriptors = bindless_element_count;
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
    void BindlessTable::setd3d12GPUDescriptorDirty(BindlessHandle _handle)
    {
        m_dirtyGPUHandle.push_back(_handle);
    }

    //--------------------------------------------------------------------------------------
    void BindlessTable::beginFrame()
    {
        VG_PROFILE_CPU("UpdateGPUDescriptors");

        auto * device = driver::Device::get();

        for (const auto & handle : m_dirtyGPUHandle)
        {
            D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptors = m_srvCPUDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
            D3D12_CPU_DESCRIPTOR_HANDLE gpuDescriptors = m_srvGPUDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
            
            cpuDescriptors.ptr += handle * m_srcDescriptorHeapSize;
            gpuDescriptors.ptr += handle * m_srcDescriptorHeapSize;

            device->getd3d12Device()->CopyDescriptorsSimple(1, gpuDescriptors, cpuDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        }

        m_dirtyGPUHandle.clear();
    }

    //--------------------------------------------------------------------------------------
    D3D12_CPU_DESCRIPTOR_HANDLE BindlessTable::getd3d12CPUDescriptorHandle(BindlessHandle _handle) const
    {
        D3D12_CPU_DESCRIPTOR_HANDLE handle = { m_srvCPUDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
        handle.ptr += _handle * m_srcDescriptorHeapSize;
        return handle;
    }

    //--------------------------------------------------------------------------------------
    D3D12_GPU_DESCRIPTOR_HANDLE BindlessTable::getd3d12GPUDescriptorHandle(BindlessHandle _handle) const
    {
        D3D12_GPU_DESCRIPTOR_HANDLE handle = { m_srvGPUDescriptorHeap->GetGPUDescriptorHandleForHeapStart() };
        handle.ptr += _handle * m_srcDescriptorHeapSize;
        return handle;
    }

    //--------------------------------------------------------------------------------------
    void BindlessTable::copyTextureHandle(core::uint _slot, driver::Texture * _texture)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE src = getd3d12CPUDescriptorHandle(_texture->getBindlessSRVHandle());
        D3D12_CPU_DESCRIPTOR_HANDLE dst = getd3d12CPUDescriptorHandle(_slot);

        auto * device = driver::Device::get();
        device->getd3d12Device()->CopyDescriptorsSimple(1, dst, src, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        setd3d12GPUDescriptorDirty(_slot);
    }
}