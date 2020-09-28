namespace vg::graphics::driver::dx12
{
    //--------------------------------------------------------------------------------------
    Buffer::Buffer(const BufferDesc & _bufDesc, const core::string & _name, void * _initData, ReservedSlot _reservedSlot) :
        base::Buffer(_bufDesc, _name, _initData)
    {
        auto * device = driver::Device::get();
        auto * d3d12device = device->getd3d12Device();   

        D3D12_RESOURCE_DESC resourceDesc = getd3d12ResourceDesc(_bufDesc);
        D3D12MA::ALLOCATION_DESC allocDesc = getd3d12AllocationDesc(_bufDesc);

        D3D12MA::Allocator * allocator = driver::Device::get()->getd3d12MemoryAllocator();
        ID3D12Resource * resource;
        D3D12MA::Allocation * alloc;
        VG_ASSERT_SUCCEEDED(allocator->CreateResource(&allocDesc, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, &alloc, IID_PPV_ARGS(&resource)));
        m_resource.setd3d12BufferResource(resource, alloc);
    }

    //--------------------------------------------------------------------------------------
    Buffer::~Buffer()
    {
        
    }

    //--------------------------------------------------------------------------------------
    D3D12_RESOURCE_DESC Buffer::getd3d12ResourceDesc(const BufferDesc & _bufDesc)
    {
        D3D12_RESOURCE_DESC resourceDesc = {};

        if (Usage::Upload == _bufDesc.resource.m_usage)
        {
            resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
            resourceDesc.Alignment = 0;
            resourceDesc.Width = _bufDesc.size();
            resourceDesc.Height = 1;
            resourceDesc.DepthOrArraySize = 1;
            resourceDesc.MipLevels = 1;
            resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
            resourceDesc.SampleDesc.Count = 1;
            resourceDesc.SampleDesc.Quality = 0;
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
        }

        return resourceDesc;
    }

    //--------------------------------------------------------------------------------------
    D3D12MA::ALLOCATION_DESC Buffer::getd3d12AllocationDesc(const BufferDesc & _bufDesc)
    {
        D3D12MA::ALLOCATION_DESC allocDesc = {};

        if (Usage::Upload == _bufDesc.resource.m_usage)
        {
            allocDesc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;
            allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
        }

        return allocDesc;
    }
}