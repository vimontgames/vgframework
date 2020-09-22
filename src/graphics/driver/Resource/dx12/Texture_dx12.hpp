namespace vg::graphics::driver::dx12
{
    //--------------------------------------------------------------------------------------
    DXGI_FORMAT Texture::getd3d12PixelFormat(PixelFormat _format)
    {
        switch (_format)
        {
            case PixelFormat::R8G8B8A8_unorm:
                return DXGI_FORMAT_R8G8B8A8_UNORM;

            default:
                VG_ASSERT(false, "Unhandled pixel format \"%s\"", asString(_format).c_str());
                return DXGI_FORMAT_UNKNOWN;
        }
    }

    //--------------------------------------------------------------------------------------
    D3D12_RESOURCE_DIMENSION Texture::getd3d12ResourceDimension(TextureType _texType)
    {
        switch (_texType)
        {
            case TextureType::Texture1D : 
            case TextureType::Texture1DArray:
                return D3D12_RESOURCE_DIMENSION_TEXTURE1D;

            default:
                VG_ASSERT(false, "Unhandled TextureType \"%s\"", asString(_texType).c_str());
            case TextureType::Texture2D:
            case TextureType::Texture2DArray:
            case TextureType::TextureCube:
            case TextureType::TextureCubeArray:
            case TextureType::Texture2DMS:
            case TextureType::Texture2DMSArray:
                return D3D12_RESOURCE_DIMENSION_TEXTURE2D;

            case TextureType::Texture3D:
                return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
        }
    }

    //--------------------------------------------------------------------------------------
    PixelFormat getPixelFormat(DXGI_FORMAT _d3d12Format)
    {
        switch (_d3d12Format)
        {
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            return PixelFormat::R8G8B8A8_unorm;

        default:
            VG_ASSERT(false, "Unhandled pixel format \"%s\"", asString(_d3d12Format).c_str());
            return PixelFormat::Unknow;
        }
    }

    //--------------------------------------------------------------------------------------
    D3D12_RESOURCE_DESC Texture::getd3d12ResourceDesc(const TextureDesc & _texDesc)
    {
        D3D12_RESOURCE_DESC resourceDesc = {};
                            resourceDesc.Dimension = getd3d12ResourceDimension(_texDesc.type);
                            resourceDesc.Alignment = 0;
                            resourceDesc.Width = _texDesc.width;
                            resourceDesc.Height = _texDesc.height;
                            resourceDesc.DepthOrArraySize = _texDesc.depth;
                            resourceDesc.MipLevels = _texDesc.mipmaps;
                            resourceDesc.Format = getd3d12PixelFormat(_texDesc.format);
                            resourceDesc.SampleDesc.Count = 1;
                            resourceDesc.SampleDesc.Quality = 0;
                            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
                            resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        return resourceDesc;
    }

	//--------------------------------------------------------------------------------------
	Texture::Texture(const TextureDesc & _texDesc, const core::string & _name, void * _initData) :
		base::Texture(_texDesc, _name, _initData)
	{
		auto * device = driver::Device::get();
        auto * d3d12device = device->getd3d12Device();
        
        if (asBool(TextureFlags::Backbuffer & _texDesc.flags))
        {
            ID3D12Resource * backbufferResource = static_cast<ID3D12Resource*>(_initData);
            VG_ASSERT(backbufferResource);
            m_resource.setd3d12TextureResource(backbufferResource, nullptr);
        }
        else //if (asBool(TextureFlags::ShaderResource & _texDesc.flags))
        {
            D3D12_RESOURCE_DESC resourceDesc = getd3d12ResourceDesc(_texDesc);

            D3D12MA::ALLOCATION_DESC allocDesc = {};
            allocDesc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;
            allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

            D3D12MA::Allocator * allocator = driver::Device::get()->getd3d12MemoryAllocator();
            ID3D12Resource * resource;
            D3D12MA::Allocation * alloc;
            VG_ASSERT_SUCCEEDED(allocator->CreateResource(&allocDesc, &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, &alloc, IID_PPV_ARGS(&resource)));
            m_resource.setd3d12TextureResource(resource, alloc);

            if (nullptr != _initData)
            {
                auto & context = device->getCurrentFrameContext();
                auto * data = context.m_uploadCur;

                const auto fmtSize = getPixelFormatSize(_texDesc.format);

                // Make a func
                D3D12_SUBRESOURCE_FOOTPRINT pitchedDesc = {};
                pitchedDesc.Format = getd3d12PixelFormat(_texDesc.format);
                pitchedDesc.Width = _texDesc.width;
                pitchedDesc.Height = _texDesc.height;
                pitchedDesc.Depth = 1;
                pitchedDesc.RowPitch = (uint)alignUp(_texDesc.width * fmtSize, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

                const u32 offset = (u32)(context.m_uploadCur - context.m_uploadBegin);

                //D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedTexture2D = { 0 };
                //placedTexture2D.Offset = context.m_uploadCur - context.m_uploadBegin;
                //placedTexture2D.Footprint = pitchedDesc;

                u64 uploadBufferSize = 0;
                d3d12device->GetCopyableFootprints(&resourceDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);
                VG_ASSERT((core::u32)uploadBufferSize == uploadBufferSize);

                // Copy to upload buffer line by line
                context.m_uploadCur = (u8*)alignUp((uint_ptr)context.m_uploadCur, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);
                
                for (uint y = 0; y < _texDesc.height; ++y)
                    memcpy(context.m_uploadCur + y * pitchedDesc.Width, &((u8*)_initData)[y * _texDesc.width * fmtSize], fmtSize * _texDesc.width);

                //D3D12_TEXTURE_COPY_LOCATION dst = {};
                //dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
                //dst.SubresourceIndex = 0;
                //dst.pResource = getResource().getd3d12TextureResource();
                //
                //D3D12_TEXTURE_COPY_LOCATION src = {};
                //src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
                //src.PlacedFootprint = placedTexture2D;
                //src.pResource = context.m_uploadBuffer->getResource().getd3d12BufferResource();

                device->upload(static_cast<driver::Texture*>(this), offset);

                context.m_uploadCur += uploadBufferSize;
            }
        }

        if (asBool(TextureFlags::RenderTarget & _texDesc.flags))
        {
            D3D12_RENDER_TARGET_VIEW_DESC viewDesc;
                                            viewDesc.Format = getd3d12PixelFormat(_texDesc.format);
                                            viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
                                            viewDesc.Texture2D.MipSlice = 0;  // TODO: alloc RTV for every mip available
                                            viewDesc.Texture2D.PlaneSlice = 0;

            m_d3d12RTVHandle = device->allocRenderTargetViewHandle();

            VG_ASSERT(m_resource.getd3d12TextureResource());
            d3d12device->CreateRenderTargetView(m_resource.getd3d12TextureResource(), &viewDesc, m_d3d12RTVHandle);
        }
	}

	//--------------------------------------------------------------------------------------
	Texture::~Texture()
	{
		auto * device = driver::Device::get();

		if (s_d3d12invalidRenderTargetViewHandle.ptr != m_d3d12RTVHandle.ptr)
			device->freeRenderTargetViewHandle(m_d3d12RTVHandle);

        if (asBool(TextureFlags::Backbuffer & getTexDesc().flags))
            m_resource.setd3d12TextureResource(nullptr, nullptr); // Do not release backbuffer
	}

	//--------------------------------------------------------------------------------------
	const D3D12_CPU_DESCRIPTOR_HANDLE & Texture::getd3d12RTVHandle() const
	{
		return m_d3d12RTVHandle;
	}
}