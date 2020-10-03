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

        D3D12_RESOURCE_STATES resourceState;

        if (asBool(Usage::Upload & _bufDesc.resource.m_usage))
            resourceState = D3D12_RESOURCE_STATE_GENERIC_READ;
        else
            resourceState = D3D12_RESOURCE_STATE_COPY_DEST;

        D3D12MA::Allocator * allocator = driver::Device::get()->getd3d12MemoryAllocator();
        ID3D12Resource * resource;
        D3D12MA::Allocation * alloc;
        VG_ASSERT_SUCCEEDED(allocator->CreateResource(&allocDesc, &resourceDesc, resourceState, nullptr, &alloc, IID_PPV_ARGS(&resource)));
        m_resource.setd3d12BufferResource(resource, alloc);

        if (asBool(BindFlags::ShaderResource & _bufDesc.resource.m_bindFlags))
        {
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;// DXGI_FORMAT_UNKNOWN;

            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
            srvDesc.Buffer.FirstElement = 0;
            srvDesc.Buffer.NumElements = _bufDesc.size() / sizeof(u32);
            srvDesc.Buffer.StructureByteStride = 0;// _bufDesc.elementSize;
            srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW; // D3D12_BUFFER_SRV_FLAG_NONE;

            VG_ASSERT(m_resource.getd3d12BufferResource());

            BindlessTable * bindlessTable = device->getBindlessTable();
            m_bindlessSRVHandle = bindlessTable->allocBindlessBufferHandle(static_cast<driver::Buffer*>(this), _reservedSlot);

            D3D12_CPU_DESCRIPTOR_HANDLE d3d12DescriptorHandle = bindlessTable->getd3d12DescriptorHandle(m_bindlessSRVHandle);
            d3d12device->CreateShaderResourceView(m_resource.getd3d12BufferResource(), &srvDesc, d3d12DescriptorHandle);
        }

        if (nullptr != _initData)
        {
            auto & context = device->getCurrentFrameContext();
            auto * data = context.m_uploadCur;

            u64 uploadBufferSize = 0;

            // Copy to upload buffer line by line
            context.m_uploadCur = (u8*)alignUp((uint_ptr)context.m_uploadCur, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
            const u32 offset = (u32)(context.m_uploadCur - context.m_uploadBegin);
            
            memcpy(context.m_uploadCur, _initData, _bufDesc.size());

            device->upload(static_cast<driver::Buffer*>(this), offset);

            context.m_uploadCur += uploadBufferSize;
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
                VG_ASSERT(false, "Unhandled Usage \"%s\" (%u)", asString(usage), usage);

            case Usage::Default:
                allocDesc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;
                allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
                break;

            case Usage::Upload:
                allocDesc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;
                allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
                break;
        }

        return allocDesc;
    }
}