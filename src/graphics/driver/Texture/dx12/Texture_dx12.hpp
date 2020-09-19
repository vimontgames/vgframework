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
	Texture::Texture(const TextureDesc & _texDesc, const core::string & _name, void * _initData) :
		base::Texture(_texDesc, _name, _initData)
	{
		auto * device = driver::Device::get();
        auto * d3d12device = device->getd3d12Device();

		if (asBool(TextureFlags::Backbuffer & _texDesc.flags))
		{
			ID3D12Resource * backbufferResource = static_cast<ID3D12Resource*>(_initData);
			VG_ASSERT(backbufferResource);
			m_d3d12resource = backbufferResource;
		}
        else //if (asBool(TextureFlags::ShaderResource & _texDesc.flags))
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

            D3D12MA::ALLOCATION_DESC allocDesc = {};
                                     allocDesc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;
                                     allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

            D3D12MA::Allocator * allocator = driver::Device::get()->getd3d12MemoryAllocator();
            VG_ASSERT_SUCCEEDED(allocator->CreateResource(&allocDesc, &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, &m_d3d12ma_alloc, IID_PPV_ARGS(&m_d3d12resource)));

            if (nullptr != _initData)
            {
                u64 stagingBufferSize = 0;
                d3d12device->GetCopyableFootprints( &resourceDesc, 0, 1, 0, nullptr, nullptr,  nullptr, &stagingBufferSize);

                // create staging buffer for texture upload
                D3D12_RESOURCE_DESC stagingBufferDesc;
                                    stagingBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
                                    stagingBufferDesc.Alignment = 0;
                                    stagingBufferDesc.Width = stagingBufferSize;
                                    stagingBufferDesc.Height = 1;
                                    stagingBufferDesc.DepthOrArraySize = 1;
                                    stagingBufferDesc.MipLevels = 1;
                                    stagingBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
                                    stagingBufferDesc.SampleDesc.Count = 1;
                                    stagingBufferDesc.SampleDesc.Quality = 0;
                                    stagingBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
                                    stagingBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
            }
        }

		if (asBool(TextureFlags::RenderTarget & _texDesc.flags))
		{
			D3D12_RENDER_TARGET_VIEW_DESC viewDesc;
										  viewDesc.Format = getd3d12PixelFormat(_texDesc.format);
										  viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
										  viewDesc.Texture2D.MipSlice = 0;
										  viewDesc.Texture2D.PlaneSlice = 0;

			m_d3d12RTVHandle = device->allocRenderTargetViewHandle();

			VG_ASSERT(m_d3d12resource);
			d3d12device->CreateRenderTargetView(m_d3d12resource, &viewDesc, m_d3d12RTVHandle);
		}
	}

	//--------------------------------------------------------------------------------------
	void Texture::setd3d12Resource(ID3D12Resource * _resource)
	{
		m_d3d12resource = _resource;
	}

	//--------------------------------------------------------------------------------------
	Texture::~Texture()
	{
		auto * device = driver::Device::get();

		if (s_d3d12invalidRenderTargetViewHandle.ptr != m_d3d12RTVHandle.ptr)
			device->freeRenderTargetViewHandle(m_d3d12RTVHandle);

        if (!asBool(TextureFlags::Backbuffer & getTexDesc().flags))
            VG_SAFE_RELEASE(m_d3d12resource);

        VG_SAFE_RELEASE(m_d3d12ma_alloc);
	}

	//--------------------------------------------------------------------------------------
	ID3D12Resource * Texture::getd3d12Resource() const 
	{ 
		return m_d3d12resource; 
	}

	//--------------------------------------------------------------------------------------
	const D3D12_CPU_DESCRIPTOR_HANDLE & Texture::getd3d12RTVHandle() const
	{
		return m_d3d12RTVHandle;
	}
}