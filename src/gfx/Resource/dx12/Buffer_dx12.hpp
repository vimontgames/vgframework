namespace vg::gfx::dx12
{
    //--------------------------------------------------------------------------------------
    Buffer::Buffer(const BufferDesc & _bufDesc, const core::string & _name, const void * _initData, ReservedSlot _reservedSlot) :
        base::Buffer(_bufDesc, _name, _initData)
    {
        auto * device = gfx::Device::get();
        auto * d3d12device = device->getd3d12Device();   

        D3D12_RESOURCE_DESC resourceDesc = getd3d12ResourceDesc(_bufDesc);
        D3D12MA::ALLOCATION_DESC allocDesc = getd3d12AllocationDesc(_bufDesc);

        D3D12_RESOURCE_STATES resourceState;

        const auto usage = _bufDesc.resource.m_usage;
        switch (usage)
        {
            case Usage::Upload:
                resourceState = D3D12_RESOURCE_STATE_GENERIC_READ;
                break;

            case Usage::Default:
                resourceState = D3D12_RESOURCE_STATE_GENERIC_READ;
                break;

            case Usage::Staging:
                resourceState = D3D12_RESOURCE_STATE_COPY_DEST;
                break;

        }

        if (asBool(BindFlags::UnorderedAccess & _bufDesc.resource.m_bindFlags))
        {
            resourceState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }

        if (_bufDesc.testBindFlags(BindFlags::RaytracingAccelerationStruct))
        {
            resourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
        }

        D3D12MA::Allocator * allocator = gfx::Device::get()->getd3d12MemoryAllocator();
        ID3D12Resource * resource;
        D3D12MA::Allocation * alloc;
        VG_VERIFY_SUCCEEDED(allocator->CreateResource(&allocDesc, &resourceDesc, resourceState, nullptr, &alloc, IID_PPV_ARGS(&resource)));
        m_resource.setd3d12BufferResource(_name, resource, alloc);

        if (asBool(BindFlags::ShaderResource & _bufDesc.resource.m_bindFlags))
        {
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;// DXGI_FORMAT_UNKNOWN;

            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
            srvDesc.Buffer.FirstElement = 0;
            srvDesc.Buffer.NumElements = _bufDesc.getSize() / sizeof(u32);
            srvDesc.Buffer.StructureByteStride = 0;// _bufDesc.elementSize;
            srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

            VG_ASSERT(m_resource.getd3d12BufferResource());

            BindlessTable * bindlessTable = device->getBindlessTable();
            m_bufferHandle = bindlessTable->allocBindlessBufferHandle(static_cast<gfx::Buffer*>(this), _reservedSlot);

            D3D12_CPU_DESCRIPTOR_HANDLE d3d12DescriptorHandle = bindlessTable->getd3d12CPUDescriptorHandle(m_bufferHandle);
            d3d12device->CreateShaderResourceView(m_resource.getd3d12BufferResource(), &srvDesc, d3d12DescriptorHandle);
            bindlessTable->updated3d12descriptor(getBufferHandle());
        }

        if (asBool(BindFlags::UnorderedAccess & _bufDesc.resource.m_bindFlags))
        {
            BindlessTable * bindlessTable = device->getBindlessTable();
            m_rwBufferHandle = bindlessTable->allocBindlessRWBufferHandle(static_cast<gfx::Buffer *>(this), _reservedSlot);
            D3D12_CPU_DESCRIPTOR_HANDLE d3d12RWBufferDescriptorHandle = bindlessTable->getd3d12CPUDescriptorHandle(m_rwBufferHandle);

            D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
            uavDesc.Buffer.FirstElement = 0;
            uavDesc.Buffer.NumElements = _bufDesc.getSize() / sizeof(u32);
            uavDesc.Buffer.StructureByteStride = 0;
            uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
            d3d12device->CreateUnorderedAccessView(m_resource.getd3d12BufferResource(), nullptr, &uavDesc, d3d12RWBufferDescriptorHandle);
            bindlessTable->updated3d12descriptor(getRWBufferHandle());
        }

        if (nullptr != _initData)
        {
            const size_t uploadBufferSize = _bufDesc.getSize();
            const auto * scheduler = Kernel::getScheduler();
            VG_ASSERT(scheduler->IsMainThread() || scheduler->IsLoadingThread(), "Expected Main or Loading thread but current thread is \"%s\"", scheduler->GetCurrentThreadName().c_str());
            auto * uploadBuffer = device->getUploadBuffer(0);

            core::u8 * dst = uploadBuffer->map((gfx::Buffer *)this, uploadBufferSize, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
            if (nullptr != dst)
                memcpy(dst, _initData, uploadBufferSize);

            uploadBuffer->unmap((gfx::Buffer*)this, dst, uploadBufferSize);
        }
    }

    //--------------------------------------------------------------------------------------
    Buffer::~Buffer()
    {
        
    }

    //--------------------------------------------------------------------------------------
    D3D12_RESOURCE_DESC Buffer::getd3d12ResourceDesc(const BufferDesc & _bufDesc)
    {
        D3D12_RESOURCE_DESC resourceDesc = {};

        auto size = _bufDesc.getSize();

        if (_bufDesc.testBindFlags(BindFlags::ConstantBuffer))
            size = (uint)alignUp(size, (u32)256);

        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Alignment = 0;
        resourceDesc.Width = size;
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		if (_bufDesc.testBindFlags(BindFlags::UnorderedAccess | BindFlags::RaytracingAccelerationStruct))
			resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

        return resourceDesc;
    }

    //--------------------------------------------------------------------------------------
    D3D12MA::ALLOCATION_DESC Buffer::getd3d12AllocationDesc(const BufferDesc & _bufDesc)
    {
        D3D12MA::ALLOCATION_DESC allocDesc = {};
        const Usage usage = _bufDesc.resource.m_usage;
        switch (usage)
        {
            default:
                VG_ASSERT(false, "Unhandled Usage \"%s\" (%u)", asString(usage).c_str(), usage);

            case Usage::Default:
                allocDesc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;
                allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
                break;

            case Usage::Upload:
                allocDesc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;
                allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
                break;

            case Usage::Staging:
                allocDesc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;
                allocDesc.HeapType = D3D12_HEAP_TYPE_READBACK;
                break;
        }

        return allocDesc;
    }
}