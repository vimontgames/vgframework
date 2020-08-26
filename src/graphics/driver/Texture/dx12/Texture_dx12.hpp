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

		if (asBool(TextureFlags::Backbuffer & _texDesc.flags))
		{
			ID3D12Resource * backbufferResource = static_cast<ID3D12Resource*>(_initData);
			VG_ASSERT(backbufferResource);
			m_d3d12resource = backbufferResource;
		}

		if (asBool(TextureFlags::RenderTarget & _texDesc.flags))
		{
			D3D12_RENDER_TARGET_VIEW_DESC viewDesc;
										  viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
										  viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
										  viewDesc.Texture2D.MipSlice = 0;
										  viewDesc.Texture2D.PlaneSlice = 0;

			m_d3d12RTVHandle = device->allocRenderTargetViewHandle();

			VG_ASSERT(m_d3d12resource);
			device->getd3d12Device()->CreateRenderTargetView(m_d3d12resource, &viewDesc, m_d3d12RTVHandle);
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