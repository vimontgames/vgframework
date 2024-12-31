namespace vg::gfx::dx12
{
    //--------------------------------------------------------------------------------------
    DXGI_FORMAT Texture::getd3d12ResourceFormat(PixelFormat _format)
    {
        switch (_format)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_format);
                return DXGI_FORMAT_UNKNOWN;

            case PixelFormat::R8G8B8A8_unorm:
                return DXGI_FORMAT_R8G8B8A8_UNORM;

            case PixelFormat::R8G8B8A8_unorm_sRGB:
                return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

            case PixelFormat::R10G10B10A2_unorm:
                return DXGI_FORMAT_R10G10B10A2_UNORM;

            case PixelFormat::R16G16B16A16_unorm:
                return DXGI_FORMAT_R16G16B16A16_UNORM;

            case PixelFormat::R16G16B16A16_float:
                return DXGI_FORMAT_R16G16B16A16_FLOAT;

            case PixelFormat::R16G16_float:
                return DXGI_FORMAT_R16G16_FLOAT;

            case PixelFormat::R32G32B32A32_float:
                return DXGI_FORMAT_R32G32B32A32_FLOAT;

            case PixelFormat::D32S8:
                return DXGI_FORMAT_R32G8X24_TYPELESS;
        }
    }

    //--------------------------------------------------------------------------------------
    DXGI_FORMAT Texture::getd3d12ShaderResourceViewFormat(PixelFormat _format, bool _stencil)
    {
        switch (_format)
        {
            default:
                return getd3d12ResourceFormat(_format);

            case PixelFormat::D32S8:
                return _stencil ? DXGI_FORMAT_X32_TYPELESS_G8X24_UINT : DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
        }
    }

    //--------------------------------------------------------------------------------------
    DXGI_FORMAT Texture::getd3d12SurfaceFormat(PixelFormat _format)
    {
        switch (_format)
        {
            default:
                return getd3d12ResourceFormat(_format);

            case PixelFormat::D32S8:
                return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        }
    }

    //--------------------------------------------------------------------------------------
    D3D12_RESOURCE_DIMENSION Texture::getd3d12ResourceDimension(TextureType _texType)
    {
        switch (_texType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_texType);

            case TextureType::Texture2D:
            case TextureType::Texture2DArray:
            case TextureType::TextureCube:
            case TextureType::TextureCubeArray:
            case TextureType::Texture2DMS:
            case TextureType::Texture2DMSArray:
                return D3D12_RESOURCE_DIMENSION_TEXTURE2D;

            case TextureType::Texture1D:
            case TextureType::Texture1DArray:
                return D3D12_RESOURCE_DIMENSION_TEXTURE1D;

            case TextureType::Texture3D:
                return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
        }
    }

    //--------------------------------------------------------------------------------------
    D3D12_SRV_DIMENSION Texture::getd3d12ShaderResourceViewDimension(TextureType _texType)
    {
        switch (_texType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_texType);

            case TextureType::Texture2D:
                return D3D12_SRV_DIMENSION_TEXTURE2D;

            case TextureType::Texture2DArray:
                return D3D12_SRV_DIMENSION_TEXTURE2DARRAY;

            case TextureType::TextureCube:
                return D3D12_SRV_DIMENSION_TEXTURECUBE;

            case TextureType::TextureCubeArray:
                return D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;

            case TextureType::Texture2DMS:
                return D3D12_SRV_DIMENSION_TEXTURE2DMS;

            case TextureType::Texture2DMSArray:
                return D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;

            case TextureType::Texture3D:
                return D3D12_SRV_DIMENSION_TEXTURE3D;

            case TextureType::Texture1D:
                return D3D12_SRV_DIMENSION_TEXTURE1D;

            case TextureType::Texture1DArray:
                return D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
        }
    }

    //--------------------------------------------------------------------------------------
    D3D12_RTV_DIMENSION Texture::getd3d12RenderTargetViewDimension(TextureType _texType)
    {
        switch (_texType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_texType);

            case TextureType::Texture2D:
                return D3D12_RTV_DIMENSION_TEXTURE2D;

            case TextureType::Texture2DArray:
                return D3D12_RTV_DIMENSION_TEXTURE2DARRAY;

            //case TextureType::TextureCube:
            //    return D3D12_RTV_DIMENSION_TEXTURECUBE;
            //
            //case TextureType::TextureCubeArray:
            //    return D3D12_RTV_DIMENSION_TEXTURECUBEARRAY;

            case TextureType::Texture2DMS:
                return D3D12_RTV_DIMENSION_TEXTURE2DMS;

            case TextureType::Texture2DMSArray:
                return D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;

            case TextureType::Texture3D:
                return D3D12_RTV_DIMENSION_TEXTURE3D;

            case TextureType::Texture1D:
                return D3D12_RTV_DIMENSION_TEXTURE1D;

            case TextureType::Texture1DArray:
                return D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
        }
    }

    //--------------------------------------------------------------------------------------
    D3D12_DSV_DIMENSION Texture::getd3d12DepthStencilViewDimension(TextureType _texType)
    {
        switch (_texType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_texType);

            case TextureType::Texture2D:
                return D3D12_DSV_DIMENSION_TEXTURE2D;

            case TextureType::Texture2DArray:
                return D3D12_DSV_DIMENSION_TEXTURE2DARRAY;

            //case TextureType::TextureCube:
            //    return D3D12_DSV_DIMENSION_TEXTURECUBE;

            //case TextureType::TextureCubeArray:
            //    return D3D12_DSV_DIMENSION_TEXTURECUBEARRAY;

            case TextureType::Texture2DMS:
                return D3D12_DSV_DIMENSION_TEXTURE2DMS;

            case TextureType::Texture2DMSArray:
                return D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;

            //case TextureType::Texture3D:
            //    return D3D12_DSV_DIMENSION_TEXTURE3D;

            case TextureType::Texture1D:
                return D3D12_DSV_DIMENSION_TEXTURE1D;

            case TextureType::Texture1DArray:
                return D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
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
                VG_ASSERT(false, "Unhandled DXGI_FORMAT %u", _d3d12Format);
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
                            resourceDesc.DepthOrArraySize = _texDesc.slices;
                            resourceDesc.MipLevels = _texDesc.mipmaps;
                            resourceDesc.Format = getd3d12ResourceFormat(_texDesc.format);
                            resourceDesc.SampleDesc.Count = 1;
                            resourceDesc.SampleDesc.Quality = 0;
                            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
                            resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        if (asBool(TextureFlags::RenderTarget & _texDesc.flags))
            resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

        if (asBool(TextureFlags::DepthStencil & _texDesc.flags))
            resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        switch (_texDesc.msaa)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_texDesc.msaa);

            case MSAA::None:
                break;

            case MSAA::MSAA2X:
            case MSAA::MSAA4X:
            case MSAA::MSAA8X:
            case MSAA::MSAA16X:
            {
                resourceDesc.SampleDesc.Count = (uint)TextureDesc::getMSAASampleCount(_texDesc.msaa);
                resourceDesc.SampleDesc.Quality = 0;
            }
            break;
        }

        return resourceDesc;
    }

	//--------------------------------------------------------------------------------------
	Texture::Texture(const TextureDesc & _texDesc, const core::string & _name, const void * _initData, ReservedSlot _reservedSlot) :
		base::Texture(_texDesc, _name, _initData)
	{
		auto * device = gfx::Device::get();
        auto * d3d12device = device->getd3d12Device();
        
        D3D12_RESOURCE_DESC resourceDesc = getd3d12ResourceDesc(_texDesc);
        DXGI_FORMAT d3d12PixFmt = getd3d12ResourceFormat(_texDesc.format);

        if (asBool(TextureFlags::Backbuffer & _texDesc.flags))
        {
            ID3D12Resource * backbufferResource = (ID3D12Resource*)_initData;
            VG_ASSERT(backbufferResource);
            m_resource.setd3d12TextureResource(_name, backbufferResource, nullptr);
            m_resource.setResourceFlags(ResourceFlags::Backbuffer);
        }
        else
        {
            D3D12MA::ALLOCATION_DESC allocDesc = {};
            allocDesc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;
            allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

            D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_COMMON;

            D3D12_CLEAR_VALUE optimizedClearValue;
            D3D12_CLEAR_VALUE * pOptimizedClearValue = nullptr;

            if (asBool(TextureFlags::RenderTarget & _texDesc.flags))
            {
                initState = D3D12_RESOURCE_STATE_RENDER_TARGET;
                optimizedClearValue.Format = getd3d12SurfaceFormat(_texDesc.format);
                optimizedClearValue.Color[0] = defaultOptimizedClearColor.x;
                optimizedClearValue.Color[1] = defaultOptimizedClearColor.y;
                optimizedClearValue.Color[2] = defaultOptimizedClearColor.z;
                optimizedClearValue.Color[3] = defaultOptimizedClearColor.w;

                pOptimizedClearValue = &optimizedClearValue;
            }
            else if (asBool(TextureFlags::DepthStencil & _texDesc.flags))
            {
                initState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
                optimizedClearValue.Format = getd3d12SurfaceFormat(_texDesc.format);
                optimizedClearValue.DepthStencil.Depth = defaultOptimizedClearDepth;
                optimizedClearValue.DepthStencil.Stencil = defaultOptimizedClearStencil;

                pOptimizedClearValue = &optimizedClearValue;
            }
            else if (_initData)
            {
                initState = D3D12_RESOURCE_STATE_COPY_DEST;
            }

            if (asBool(BindFlags::UnorderedAccess & _texDesc.resource.m_bindFlags))
            {
                if (!asBool(TextureFlags::RenderTarget & _texDesc.flags))
                    initState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

                resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
            }

            D3D12MA::Allocator * allocator = gfx::Device::get()->getd3d12MemoryAllocator();
            ID3D12Resource * resource;
            D3D12MA::Allocation * alloc;
            VG_VERIFY_SUCCEEDED(allocator->CreateResource(&allocDesc, &resourceDesc, initState, pOptimizedClearValue, &alloc, IID_PPV_ARGS(&resource)));
            m_resource.setd3d12TextureResource(_name, resource, alloc);
        }

        if (asBool(BindFlags::ShaderResource & _texDesc.resource.m_bindFlags)) 
        {
            auto created3d12ShaderResourceView = [&](const TextureDesc & _texDesc, bool _stencil = false)
            {
                D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
                srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                srvDesc.Format = getd3d12ShaderResourceViewFormat(_texDesc.format, _stencil);
                srvDesc.ViewDimension = getd3d12ShaderResourceViewDimension(_texDesc.type);

                switch (_texDesc.type)
                {
                    default:
                        VG_ASSERT_ENUM_NOT_IMPLEMENTED(_texDesc.type);
                        break;

                    case TextureType::Texture1D:
                        srvDesc.Texture1D.MipLevels = 1;
                        srvDesc.Texture1D.MostDetailedMip = 0;
                        srvDesc.Texture1D.ResourceMinLODClamp = 0;
                        break;

                    case TextureType::Texture2D:
                        srvDesc.Texture2D.MipLevels = _texDesc.mipmaps;
                        srvDesc.Texture2D.MostDetailedMip = 0;
                        srvDesc.Texture2D.PlaneSlice = _stencil ? 1 : 0;
                        srvDesc.Texture2D.ResourceMinLODClamp = 0;
                        break;

                    case TextureType::Texture2DMS:
                        srvDesc.Texture2DMS;
                        break;

                    case TextureType::TextureCube:
                        srvDesc.TextureCube.MipLevels = _texDesc.mipmaps;
                        srvDesc.TextureCube.MostDetailedMip = 0;
                        srvDesc.TextureCube.ResourceMinLODClamp = 0;
                        break;
                }

                VG_ASSERT(m_resource.getd3d12TextureResource());

                BindlessTable * bindlessTable = device->getBindlessTable();
                BindlessTextureHandle srvHandle = bindlessTable->allocBindlessTextureHandle(static_cast<gfx::Texture *>(this), _reservedSlot);

                D3D12_CPU_DESCRIPTOR_HANDLE d3d12DescriptorHandle = bindlessTable->getd3d12CPUDescriptorHandle(srvHandle);
                d3d12device->CreateShaderResourceView(m_resource.getd3d12TextureResource(), &srvDesc, d3d12DescriptorHandle);

                return srvHandle;
            };

            if (asBool(TextureFlags::DepthStencil & _texDesc.flags))
            {
                m_depthTextureHandle = created3d12ShaderResourceView(_texDesc, false);
                m_stencilTextureHandle = created3d12ShaderResourceView(_texDesc, true);
            }
            else
            {
                m_textureHandle = created3d12ShaderResourceView(_texDesc);
            }
        }

        if (asBool(TextureFlags::RenderTarget & _texDesc.flags))
        {
            D3D12_RENDER_TARGET_VIEW_DESC viewDesc;
                                          viewDesc.Format = d3d12PixFmt;
                                          viewDesc.ViewDimension = getd3d12RenderTargetViewDimension(_texDesc.type);
                                          viewDesc.Texture2D.MipSlice = 0;  // TODO: alloc RTV for every mip available
                                          viewDesc.Texture2D.PlaneSlice = 0;

            VG_ASSERT(m_resource.getd3d12TextureResource());
            m_d3d12RTVHandle = device->allocRTVHandle();
            d3d12device->CreateRenderTargetView(m_resource.getd3d12TextureResource(), &viewDesc, m_d3d12RTVHandle);
        }
        else if (asBool(TextureFlags::DepthStencil & _texDesc.flags))
        {
            D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
                                          depthStencilViewDesc.Format = getd3d12SurfaceFormat(_texDesc.format); 
                                          depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
                                          depthStencilViewDesc.ViewDimension = getd3d12DepthStencilViewDimension(_texDesc.type);
                                          depthStencilViewDesc.Texture2D.MipSlice = 0;  // TODO: alloc DSV for every mip available

            VG_ASSERT(m_resource.getd3d12TextureResource());
            m_d3d12DSVHandle = device->allocDSVHandle();
            d3d12device->CreateDepthStencilView(m_resource.getd3d12TextureResource(), &depthStencilViewDesc, m_d3d12DSVHandle);
        }

        // Update ShaderResource descriptors in bindless table
        if (nullptr == _initData && asBool(BindFlags::ShaderResource & _texDesc.resource.m_bindFlags))
        {
            auto * bindlessTable = device->getBindlessTable();

            if (isDepthStencilFormat(_texDesc.format))
            {
                bindlessTable->updated3d12descriptor(getDepthTextureHandle());
                bindlessTable->updated3d12descriptor(getStencilTextureHandle());
            }
            else
            {
                bindlessTable->updated3d12descriptor(getTextureHandle());
            }            
        }

        if (asBool(BindFlags::UnorderedAccess & _texDesc.resource.m_bindFlags))
        {
            switch (_texDesc.type)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(_texDesc.type);
                    break;

                case TextureType::TextureCube:
                {
                    VG_ASSERT(_texDesc.slices == 6);
                    m_rwTextureMipHandles.resize(_texDesc.mipmaps);

                    for (uint m = 0; m < _texDesc.mipmaps; ++m)
                    {
                        BindlessTable * bindlessTable = device->getBindlessTable();
                        auto rwTextureHandle = bindlessTable->allocBindlessRWTextureHandle(static_cast<gfx::Texture *>(this));
                        D3D12_CPU_DESCRIPTOR_HANDLE d3d12RWTextureDescriptorHandle = bindlessTable->getd3d12CPUDescriptorHandle(rwTextureHandle);

                        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
                        uavDesc.Format = getd3d12ResourceFormat(_texDesc.format);
                        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
                        uavDesc.Texture2DArray.MipSlice = m;
                        uavDesc.Texture2DArray.FirstArraySlice = 0;
                        uavDesc.Texture2DArray.ArraySize = _texDesc.slices;
                        uavDesc.Texture2DArray.PlaneSlice = 0;
                        d3d12device->CreateUnorderedAccessView(m_resource.getd3d12TextureResource(), nullptr, &uavDesc, d3d12RWTextureDescriptorHandle);
                        bindlessTable->updated3d12descriptor(rwTextureHandle);

                        m_rwTextureMipHandles[m] = rwTextureHandle;
                    }
                }
                break;

                case TextureType::Texture2D:
                {
                    VG_ASSERT(_texDesc.slices == 1);
                    m_rwTextureMipHandles.resize(_texDesc.mipmaps);

                    for (uint m = 0; m < _texDesc.mipmaps; ++m)
                    {
                        BindlessTable * bindlessTable = device->getBindlessTable();
                        auto rwTextureHandle = bindlessTable->allocBindlessRWTextureHandle(static_cast<gfx::Texture *>(this));
                        D3D12_CPU_DESCRIPTOR_HANDLE d3d12RWTextureDescriptorHandle = bindlessTable->getd3d12CPUDescriptorHandle(rwTextureHandle);

                        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
                        uavDesc.Format = getd3d12ResourceFormat(_texDesc.format);
                        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
                        uavDesc.Texture2D.MipSlice = m;
                        uavDesc.Texture2D.PlaneSlice = 0;
                        d3d12device->CreateUnorderedAccessView(m_resource.getd3d12TextureResource(), nullptr, &uavDesc, d3d12RWTextureDescriptorHandle);
                        bindlessTable->updated3d12descriptor(rwTextureHandle);

                        m_rwTextureMipHandles[m] = rwTextureHandle;
                    }
                }
                break;
            }            
        }

        if (nullptr != _initData && !_texDesc.isBackbuffer())
        {
            auto & context = device->getCurrentFrameContext();

            const auto fmtSize = getPixelFormatSize(_texDesc.format);
            if (-1 != fmtSize)
            {
                const u32 subResourceCount = _texDesc.mipmaps * _texDesc.slices;

                vector<D3D12_SUBRESOURCE_DATA> subResource(subResourceCount);
                vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> footprint(subResourceCount);
                vector<uint> rows(subResourceCount);
                vector<u64> strides(subResourceCount);

                u8 * mipInitData = (u8 *)_initData;

                const uint width = _texDesc.width;
                const uint height = _texDesc.height;

                // Subresource Index=MipLevel+(ArrayLayer×MipLevels)
                for (uint s = 0; s < _texDesc.slices; ++s)
                {
                    uint mipWidth = width;
                    uint mipHeight = height;

                    for (uint m = 0; m < _texDesc.mipmaps; ++m)
                    {
                        const uint subResourceIndex = s * _texDesc.mipmaps + m;

                        subResource[subResourceIndex].pData = mipInitData;
                        subResource[subResourceIndex].RowPitch = mipWidth * fmtSize;    // TODO: compute pitch in bits>>3 from format and width to handle compressed formats
                        subResource[subResourceIndex].SlicePitch = subResource[subResourceIndex].RowPitch * mipHeight;

                        mipInitData += subResource[subResourceIndex].SlicePitch;

                        mipWidth >>= 1;
                        mipHeight >>= 1;
                    }
                }

                size_t d3d12TotalSizeInBytes = 0;
                d3d12device->GetCopyableFootprints(&resourceDesc, 0, subResourceCount, 0, footprint.data(), rows.data(), strides.data(), &d3d12TotalSizeInBytes);

                // Save offset to subresource for upload
                for (uint i = 0; i < subResourceCount; ++i)
                    setSubResourceData(i, footprint[i].Offset);

                // Copy to upload buffer line by line
                const auto * scheduler = Kernel::getScheduler();
                VG_ASSERT(scheduler->IsMainThread() || scheduler->IsLoadingThread(), "Expected Main or Loading thread but current thread is \"%s\"", scheduler->GetCurrentThreadName().c_str());
                auto * uploadBuffer = device->getUploadBuffer(0);
                core::u8 * dst = uploadBuffer->map((gfx::Texture *)this, d3d12TotalSizeInBytes, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);
                if (nullptr != dst)
                {
                    for (uint i = 0; i < subResourceCount; ++i)
                    {
                        for (uint y = 0; y < rows[i]; ++y)
                            memcpy(dst + footprint[i].Offset + footprint[i].Footprint.RowPitch * y, (u8 *)subResource[i].pData + subResource[i].RowPitch * y, strides[i]);
                    }
                }
                uploadBuffer->unmap((gfx::Texture *)this, dst, d3d12TotalSizeInBytes);
            }
        }
	}

    //--------------------------------------------------------------------------------------
    void Texture::setSubResourceData(uint _index, core::size_t _offset)
    {
        if (_index >= m_subResourceData.size())
            m_subResourceData.resize(_index + 1);

        m_subResourceData[_index] = { _offset };
    }

    //--------------------------------------------------------------------------------------
    const SubResourceData & Texture::getSubResourceData(core::uint _index) const
    {
        return m_subResourceData[_index];
    }

	//--------------------------------------------------------------------------------------
	Texture::~Texture()
	{
		auto * device = gfx::Device::get();

		if (s_d3d12invalidRTVHandle.ptr != m_d3d12RTVHandle.ptr)
			device->freeRTVHandle(m_d3d12RTVHandle);

        if (s_d3d12invalidDSVHandle.ptr != m_d3d12DSVHandle.ptr)
            device->freeDSVHandle(m_d3d12DSVHandle);

        if (asBool(TextureFlags::Backbuffer & getTexDesc().flags))
            m_resource.setd3d12TextureResource("", nullptr, nullptr); // Do not release backbuffer
	}

	//--------------------------------------------------------------------------------------
	const D3D12_CPU_DESCRIPTOR_HANDLE & Texture::getd3d12RTVHandle() const
	{
		return m_d3d12RTVHandle;
	}

    //--------------------------------------------------------------------------------------
    const D3D12_CPU_DESCRIPTOR_HANDLE & Texture::getd3d12DSVHandle() const
    {
        return m_d3d12DSVHandle;
    }
}