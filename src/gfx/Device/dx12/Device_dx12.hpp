#ifdef VG_DEBUG
//#define GPU_FENCE_DEBUG 1
#endif

namespace vg::gfx::dx12
{
	//--------------------------------------------------------------------------------------
	void Device::init(const DeviceParams & _params)
	{
		super::init(_params);

        uint dxgiFactoryFlags = 0;
		if (_params.debugDevice)
		{
			VG_VERIFY_SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_d3d12debug)));

			if (m_d3d12debug)
				m_d3d12debug->EnableDebugLayer();

            dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG; 
		}
		VG_VERIFY_SUCCEEDED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_dxgiFactory)));

		const D3D_FEATURE_LEVEL levels[] =
		{
            D3D_FEATURE_LEVEL_12_2,
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		IDXGIAdapter1 * adapter = nullptr;
        D3D12Device * device = nullptr;

        char description[256];
        size_t s;

        D3D12DXGIFactory * factory6 = nullptr;
        VG_ASSERT(SUCCEEDED(m_dxgiFactory->QueryInterface(IID_PPV_ARGS(&factory6))));

        for (uint l = 0; l < countof(levels); ++l)
        {
            for (uint a = 0; !m_d3d12device && SUCCEEDED(factory6->EnumAdapterByGpuPreference(a, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter))); ++a)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                // Don't select the Basic Render Driver adapter.
                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                    continue;

                if (SUCCEEDED(D3D12CreateDevice(adapter, levels[l], IID_PPV_ARGS(&device))))
                {
                    adapter->GetDesc1(&desc);

                    m_d3d12device = device;
                    m_dxgiAdapter = adapter;
                    m_caps.d3d12.featureLevel = levels[l];
                    wcstombs_s(&s, description, countof(description), desc.Description, wcslen(desc.Description));
                }
                else
                {
                    adapter->Release();
                }
            }
        }

        factory6->Release();
     
		VG_ASSERT(device, "[Device] Could not create DirectX device\n");

		if (_params.debugDevice)
		{
			ID3D12InfoQueue * infoQueue = nullptr;
			if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
			{
				D3D12_MESSAGE_SEVERITY severities[] =
				{
					D3D12_MESSAGE_SEVERITY_INFO
				};

				D3D12_MESSAGE_ID messagesIDs[] =
				{
					D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
					D3D12_MESSAGE_ID_CREATEGRAPHICSPIPELINESTATE_RENDERTARGETVIEW_NOT_SET
				};

				D3D12_INFO_QUEUE_FILTER filter = {};
										filter.DenyList.NumSeverities = (uint)countof(severities);
										filter.DenyList.pSeverityList = severities;
										filter.DenyList.NumIDs = (uint)countof(messagesIDs);
										filter.DenyList.pIDList = messagesIDs;

				infoQueue->PushStorageFilter(&filter);
				{
					infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
					infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, _params.breakOnErrors);
					infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, _params.breakOnWarnings);
					infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, false);
					infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_MESSAGE, false);
				}
				infoQueue->Release();
			}
		}

        // Check max supported signature version
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
        if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
        {
            m_caps.d3d12.rootSignatureVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }
        else
        {
            m_caps.d3d12.rootSignatureVersion = featureData.HighestVersion;
        }

        // Check for the highest supported shader model
        D3D12_FEATURE_DATA_SHADER_MODEL shaderModelData = {};
        shaderModelData.HighestShaderModel = D3D_SHADER_MODEL_6_6;        
        if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModelData, sizeof(shaderModelData))))
        {
            m_caps.d3d12.shaderModel = D3D_SHADER_MODEL_5_1;
        }
        else
        {
            m_caps.d3d12.shaderModel = shaderModelData.HighestShaderModel;
        }

        // Shader model from caps
        m_caps.shaderModel = (gfx::ShaderModel)m_caps.d3d12.shaderModel;

        // Check raytracing support
		D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {};
        if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5))))
        {
            m_caps.rayTracing = false;
            m_caps.d3d12.raytracier_tier = D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
        }
        else
		{
			m_caps.rayTracing = options5.RaytracingTier >= D3D12_RAYTRACING_TIER_1_0;
			m_caps.d3d12.raytracier_tier = options5.RaytracingTier;
		}

        checkHDRSupport();

        uint major = (m_caps.d3d12.featureLevel & 0xF000) >> 12;
        uint minor = (m_caps.d3d12.featureLevel & 0x0F00) >> 8;
        VG_INFO("[Device] DirectX %u.%u %s- %s - %s", major, minor, m_d3d12debug ? "debug " : "", asString(m_caps.shaderModel).c_str(), description);

        m_memoryAllocator = new gfx::MemoryAllocator();

        setStablePowerState();

		auto * graphicsQueue = createCommandQueue(CommandQueueType::Graphics);
		auto * swapChain = created3d12SwapChain((HWND)_params.window, _params.resolution.x, _params.resolution.y);

		// Create fences for each frame so we can protect resources and wait for any given frame
        m_nextFrameFence = 1;
        m_nextFrameIndex = 0;
        m_d3d12fenceEvent = CreateEvent(nullptr, false, false, nullptr);
        VG_VERIFY_SUCCEEDED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_d3d12fence)));

        // Rendertarget CPU descriptor heap
        {
            D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
            heapDesc.NumDescriptors = (uint)m_RTVHandleIndexPool.allocated();
            heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            VG_VERIFY_SUCCEEDED(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_RTVDescriptorHeap)));
            m_RTVDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        }

        // DepthStencil CPU descriptor heap
        {
            D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
            heapDesc.NumDescriptors = (uint)m_DSVHandleIndexPool.allocated();
            heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            VG_VERIFY_SUCCEEDED(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_DSVDescriptorHeap)));
            m_DSVDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        }

		// For now just create command pools to handle latency
		// We'll need later to have one per thread recording command buffers
		for (uint i = 0; i < max_frame_latency; ++i)
			createFrameContext(i);

        createUploadBuffer();
        created3d12Backbuffers();

        m_bindlessTable = new gfx::BindlessTable();
	}

    //--------------------------------------------------------------------------------------
    // To detect HDR support, we will need to check the color space in the primary DXGI output associated with the app at
    // this point in time (using window/display intersection). 
    // Compute the overlay area of two rectangles, A and B.
    // (ax1, ay1) = left-top coordinates of A; (ax2, ay2) = right-bottom coordinates of A
    // (bx1, by1) = left-top coordinates of B; (bx2, by2) = right-bottom coordinates of B
    //--------------------------------------------------------------------------------------
    inline int computeIntersectionArea(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2)
    {
        return max(0, min(ax2, bx2) - max(ax1, bx1)) * max(0, min(ay2, by2) - max(ay1, by1));
    }

    //--------------------------------------------------------------------------------------
    // https://learn.microsoft.com/en-us/samples/microsoft/directx-graphics-samples/d3d12-hdr-sample-win32/
    //--------------------------------------------------------------------------------------
    void Device::checkHDRSupport()
    {
        if (!m_dxgiFactory->IsCurrent())
            VG_VERIFY_SUCCEEDED(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_dxgiFactory)));

        IDXGIAdapter1 * adapter;
        VG_VERIFY_SUCCEEDED(m_dxgiFactory->EnumAdapters1(0, &adapter));
        
        UINT i = 0;
        IDXGIOutput * currentOutput;
        IDXGIOutput * bestOutput = nullptr;
        float bestIntersectArea = -1;
        
        RECT bounds = {};
        GetWindowRect((HWND)m_deviceParams.window, &bounds);
        
        while (adapter->EnumOutputs(i, &currentOutput) != DXGI_ERROR_NOT_FOUND)
        {
            // Get the rectangle bounds of the window
            int ax1 = bounds.left;
            int ay1 = bounds.top;
            int ax2 = bounds.right;
            int ay2 = bounds.bottom;
        
            // Get the rectangle bounds of current output
            DXGI_OUTPUT_DESC desc;
            VG_VERIFY_SUCCEEDED(currentOutput->GetDesc(&desc));
            RECT r = desc.DesktopCoordinates;
            int bx1 = r.left;
            int by1 = r.top;
            int bx2 = r.right;
            int by2 = r.bottom;
        
            // Compute the intersection
            int intersectArea = computeIntersectionArea(ax1, ay1, ax2, ay2, bx1, by1, bx2, by2);
            if (intersectArea > bestIntersectArea)
            {
                bestOutput = currentOutput;
                bestIntersectArea = static_cast<float>(intersectArea);
            }
            else
                currentOutput->Release();
        
            i++;
        }
        
        bool hdr10Support = false;
        bool hdr16Support = false;
        
        // Having determined the output (display) upon which the app is primarily being 
        // rendered, retrieve the HDR capabilities of that display by checking the color space.
        if (bestOutput)
        {
            IDXGIOutput6 * output6 = nullptr;
            HRESULT hr = bestOutput->QueryInterface(__uuidof(IDXGIOutput6), reinterpret_cast<void **>(&output6));
            if (SUCCEEDED(hr)) 
            {                
                DXGI_OUTPUT_DESC1 desc1 = {};
                VG_VERIFY_SUCCEEDED(output6->GetDesc1(&desc1));
        
                hdr10Support = (desc1.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);

                D3D12_FEATURE_DATA_FORMAT_SUPPORT Support =
                {
                    DXGI_FORMAT_R16G16B16A16_FLOAT
                };

                // ...
                if (hdr10Support)
                {
                    if (SUCCEEDED(m_d3d12device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &Support, sizeof(Support))))
                    {
                        if (D3D12_FORMAT_SUPPORT1_DISPLAY & Support.Support1)
                            hdr16Support = true;
                    }
                }
        
                output6->Release();
            }
            else 
            {
                VG_ASSERT(false, "IDXGIOutput6 not be supported");
            }

            bestOutput->Release();
        }
        
        m_caps.hdr[asInteger(HDR::HDR10)] = hdr10Support;
        m_caps.hdr[asInteger(HDR::HDR16)] = hdr16Support;
                
        adapter->Release();
    }

    //--------------------------------------------------------------------------------------
    // Look in the Windows Registry to determine if Developer Mode is enabled
    //--------------------------------------------------------------------------------------
    bool Device::isDeveloperModeEnabled() const
    {
        HKEY hKey;
        LSTATUS result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AppModelUnlock", 0, KEY_READ, &hKey);
        if (result == ERROR_SUCCESS)
        {
            DWORD keyValue, keySize = sizeof(DWORD);
            result = RegQueryValueEx(hKey, L"AllowDevelopmentWithoutDevLicense", 0, NULL, (byte *)&keyValue, &keySize);
            if (result == ERROR_SUCCESS && keyValue == 1)
                return true;
            RegCloseKey(hKey);
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Device::setStablePowerState()
    {
        bool devMode = isDeveloperModeEnabled();        

        if (devMode)
            m_d3d12device->SetStablePowerState(true);
        else
            VG_WARNING("[Profiler] Profiling results might be inconsistent when Developer Mode is not enabled");

        return devMode;
    }

	//--------------------------------------------------------------------------------------
    D3D12DXGISwapChain * Device::created3d12SwapChain(HWND _winHandle, core::uint _width, core::uint _height)
	{
		VG_ASSERT(core::invalidWindowHandle != _winHandle);

		::ZeroMemory(&m_dxgiSwapChainDesc, sizeof(m_dxgiSwapChainDesc));

        m_backbufferFormat = getHDRBackbufferFormat(m_HDRMode);
        m_ColorSpace = getHDRColorSpace(m_HDRMode);

		const bool fullScreen = false; // TODO

		m_dxgiSwapChainDesc.Width = _width;
		m_dxgiSwapChainDesc.Height = _height;
		m_dxgiSwapChainDesc.Format = Texture::getd3d12ResourceFormat(m_backbufferFormat);
		m_dxgiSwapChainDesc.Stereo = false;
		m_dxgiSwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		m_dxgiSwapChainDesc.SampleDesc.Quality = 0;
		m_dxgiSwapChainDesc.SampleDesc.Count = 1;
		m_dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		m_dxgiSwapChainDesc.BufferCount = max_backbuffer_count;
		m_dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		m_dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		//m_dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING /*| DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT*/;
        m_dxgiSwapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        m_dxgiSwapChainDesc.Flags |= fullScreen ? DXGI_SWAP_CHAIN_FLAG_FULLSCREEN_VIDEO : 0;

		auto * graphicsQueue = getCommandQueue(CommandQueueType::Graphics);

		VG_VERIFY_SUCCEEDED(m_dxgiFactory->CreateSwapChainForHwnd(graphicsQueue->getd3d12CommandQueue(), _winHandle, &m_dxgiSwapChainDesc, nullptr, nullptr, (IDXGISwapChain1**)&m_dxgiSwapChain));
        
        VG_VERIFY_SUCCEEDED(m_dxgiFactory->MakeWindowAssociation(_winHandle, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_PRINT_SCREEN));

        m_currentBackbufferIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();

        if (m_dxgiSwapChainDesc.Flags & DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT)
            VG_VERIFY_SUCCEEDED(m_dxgiSwapChain->SetMaximumFrameLatency(max_frame_latency));

        applyColorSpace(m_ColorSpace);

		return m_dxgiSwapChain;
	}

    //--------------------------------------------------------------------------------------
    UINT64 WaitForFence(ID3D12Fence *Fence, HANDLE FenceEvent, UINT64 WaitValue)
    {
        UINT64 CompletedValue;
        while ((CompletedValue = Fence->GetCompletedValue()) < WaitValue)
        {
            if (FenceEvent)
            {
                WaitForSingleObject(FenceEvent, INFINITE);
            }
            else
            {
                Sleep(1);
            }
        }
        return CompletedValue;
    }

	//--------------------------------------------------------------------------------------
	D3D12_CPU_DESCRIPTOR_HANDLE Device::allocRTVHandle(core::uint _count)
	{
		VG_ASSERT(0 != m_RTVDescriptorSize);

        const auto index = m_RTVHandleIndexPool.alloc();
		D3D12_CPU_DESCRIPTOR_HANDLE handle = { m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
		handle.ptr += index * m_RTVDescriptorSize;

		return handle;
	}

	//--------------------------------------------------------------------------------------
	void Device::freeRTVHandle(D3D12_CPU_DESCRIPTOR_HANDLE & _hRTV)
	{
        D3D12_CPU_DESCRIPTOR_HANDLE start = { m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
        const auto index = (_hRTV.ptr - start.ptr) / m_RTVDescriptorSize;
        m_RTVHandleIndexPool.dealloc((u16)index);
	}

    //--------------------------------------------------------------------------------------
    D3D12_CPU_DESCRIPTOR_HANDLE Device::allocDSVHandle(core::uint _count)
    {
        VG_ASSERT(0 != m_DSVDescriptorSize);

        const auto index = m_DSVHandleIndexPool.alloc();
        D3D12_CPU_DESCRIPTOR_HANDLE handle = { m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
        handle.ptr += index * m_DSVDescriptorSize;

        return handle;
    }

    //--------------------------------------------------------------------------------------
    void Device::freeDSVHandle(D3D12_CPU_DESCRIPTOR_HANDLE & _hDSV)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE start = { m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
        const auto index = (_hDSV.ptr - start.ptr) / m_DSVDescriptorSize;
        m_DSVHandleIndexPool.dealloc((u16)index);
    }

    //--------------------------------------------------------------------------------------
    // Wait for everything to finish
    //--------------------------------------------------------------------------------------
    void Device::waitGPUIdle()
    {
        for (uint q = 0; q < enumCount<CommandQueueType>(); ++q)
        {
            const auto cmdQueueType = (CommandQueueType)q;
            auto * queue = getCommandQueue(cmdQueueType);

            // TODO: separate fence for each queue ?
            if (CommandQueueType::Graphics == cmdQueueType)
            {
                auto * d3d12queue = queue->getd3d12CommandQueue();

                u64 exit_fence = m_nextFrameFence++;

                #if DEBUG_GPU_CPU_SYNC
                VG_DEBUGPRINT("[Device::waitGPUIdle %u] Signal(%u)\n", m_frameCounter & 0xFF, exit_fence);
                #endif
                
                d3d12queue->Signal(m_d3d12fence, exit_fence);
                m_d3d12fence->SetEventOnCompletion(exit_fence, m_d3d12fenceEvent);
                
                #if DEBUG_GPU_CPU_SYNC
                VG_DEBUGPRINT("[Device::waitGPUIdle %u] Signal(%u)\n", m_frameCounter & 0xFF, exit_fence);
                #endif

                WaitForFence(m_d3d12fence, m_d3d12fenceEvent, exit_fence);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void Device::applyVSync(VSync mode)
    {
        // Nothing to do, DirectX12 is convenient API and it will be handled gracefully by 'Present'
    }

    //--------------------------------------------------------------------------------------
    void Device::applyHDR(HDR _mode)
    {
        // Nothing to do, m_HDRModeRequested != m_HDRMode will trigger backbuffer update after swap
    }

    //--------------------------------------------------------------------------------------
    DXGI_COLOR_SPACE_TYPE getd3d12ColorSpace(ColorSpace _mode)
    {
        switch (_mode)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_mode);

            case ColorSpace::Rec709:
                return DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;

            case ColorSpace::ST2084:
                return DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;

            case ColorSpace::Rec2020:
                return DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709;
        }
    }

    //--------------------------------------------------------------------------------------
    void Device::applyColorSpace(ColorSpace _mode)
    {
        const auto d3d12ColorSpace = getd3d12ColorSpace(_mode);

        UINT colorSpaceSupport = 0;
        VG_ASSERT(SUCCEEDED(m_dxgiSwapChain->CheckColorSpaceSupport(d3d12ColorSpace, &colorSpaceSupport)));
        if (colorSpaceSupport && ((colorSpaceSupport & DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT) == DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT))
        {
            VG_ASSERT(SUCCEEDED(m_dxgiSwapChain->SetColorSpace1(d3d12ColorSpace)));
            VG_INFO("[Device] Use %s ColorSpace", asString(_mode).c_str());
        }
        else
        {
            VG_ERROR("[Device] %s ColorSpace is not supported", asString(_mode).c_str());
        }
    }    

	//--------------------------------------------------------------------------------------
	void Device::deinit()
	{
        destroyUploadBuffer();

        for (uint i = 0; i < countof(m_frameContext); ++i)
            destroyFrameContext(i);

        destroyd3d12Backbuffers();

		VG_SAFE_RELEASE(m_RTVDescriptorHeap);
        VG_SAFE_RELEASE(m_DSVDescriptorHeap);
        VG_SAFE_RELEASE(m_d3d12fence);
		VG_SAFE_RELEASE(m_dxgiSwapChain);

		destroyCommandQueues();
		
        VG_SAFE_DELETE(m_bindlessTable);

		VG_SAFE_RELEASE(m_dxgiFactory);
		VG_SAFE_RELEASE(m_d3d12debug);
		VG_SAFE_RELEASE(m_dxgiAdapter);
        VG_SAFE_RELEASE(m_memoryAllocator);
		VG_SAFE_RELEASE(m_d3d12device);

		IDXGIDebug1 * debug = nullptr;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
		{
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL);
			debug->Release();
		}

		super::deinit();
	}

    //--------------------------------------------------------------------------------------
    void Device::resize(core::uint _width, core::uint _height)
    {
        destroyd3d12Backbuffers();

        m_dxgiSwapChainDesc.Width = _width;
        m_dxgiSwapChainDesc.Height = _height;

        VG_VERIFY_SUCCEEDED(m_dxgiSwapChain->ResizeBuffers(max_backbuffer_count, m_dxgiSwapChainDesc.Width, m_dxgiSwapChainDesc.Height, Texture::getd3d12ResourceFormat(m_backbufferFormat), m_dxgiSwapChainDesc.Flags));

        created3d12Backbuffers();
    }

    //--------------------------------------------------------------------------------------
    void Device::created3d12Backbuffers()
    {
        for (uint i = 0; i < max_backbuffer_count; ++i)
        {
            ID3D12Resource * d3d12backbufferResource = nullptr;
            m_dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&d3d12backbufferResource));
            createBackbuffer(i, d3d12backbufferResource);
            d3d12backbufferResource->Release();
        }
    }

    //--------------------------------------------------------------------------------------
    void Device::destroyd3d12Backbuffers()
    {
        for (uint i = 0; i < countof(m_bufferContext); ++i)
            destroyBackbuffer(i);
    }

	//--------------------------------------------------------------------------------------
    D3D12Device * Device::getd3d12Device() const
	{
		return m_d3d12device;
	}

    //--------------------------------------------------------------------------------------
    IDXGIAdapter1 * Device::getd3d12Adapter() const
    {
        return m_dxgiAdapter;
    }

    //--------------------------------------------------------------------------------------
    D3D12MA::Allocator * Device::getd3d12MemoryAllocator() const
    {
        return m_memoryAllocator->getd3d12MemoryAllocator();
    }

	//--------------------------------------------------------------------------------------
	void Device::beginFrame()
	{
		super::beginFrame();

        // Get/Increment the fence counter
        const u64 FrameFence = m_nextFrameFence;
        m_nextFrameFence = m_nextFrameFence + 1;

        const uint FrameIndex = m_nextFrameIndex;
        m_nextFrameIndex = (m_nextFrameIndex + 1) % (UINT)max_frame_latency;

        m_currentBackbufferIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();

        // Wait for the last frame occupying this slot to be complete
        {
            VG_PROFILE_CPU("Wait Fence");
            FrameContext * Frame = &m_frameContext[FrameIndex];

            #if DEBUG_GPU_CPU_SYNC
            VG_DEBUGPRINT("[Device::beginFrame %u] WaitForFence(%u)\n", m_frameCounter & 0xFF, Frame->mFrameFenceId);
            #endif

            WaitForFence(m_d3d12fence, m_d3d12fenceEvent, Frame->mFrameFenceId);
            Frame->mFrameFenceId = FrameFence;
        }
        m_currentFrameIndex = FrameIndex;

		auto & context = getCurrentFrameContext();

		for (auto & cmdPool : context.commandPools)
			cmdPool->beginFrame();

		for (uint type = 0; type < enumCount<CommandListType>(); ++type)
			for (auto & cmdList : context.commandLists[type])
				cmdList->reset();

		auto * commandList = context.commandLists[asInteger(CommandListType::Graphics)][0]->getd3d12GraphicsCommandList();

		// Transition back buffer
		D3D12_RESOURCE_BARRIER barrier;
		barrier.Transition.pResource = m_bufferContext[m_currentBackbufferIndex].backbuffer->getResource().getd3d12TextureResource();
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		commandList->ResourceBarrier(1, &barrier);
	}

	//--------------------------------------------------------------------------------------
	void Device::endFrame()
	{
        auto & context = getCurrentFrameContext();
        const auto curBackbuffer = m_dxgiSwapChain->GetCurrentBackBufferIndex();
        VG_ASSERT(m_currentBackbufferIndex == curBackbuffer);

        for (uint q = 0; q < enumCount<CommandQueueType>(); ++q)
        {
            const auto cmdQueueType = (CommandQueueType)q;
            auto * queue = getCommandQueue(cmdQueueType);
            auto & queueCmdLists = context.commandLists[q];

            vector<ID3D12CommandList*> cmdListsToExecute;

            for (uint c = 0; c < queueCmdLists.size(); ++c)
            {
                const auto cmdListType = (CommandListType)c;
                auto * d3d12cmdList = queueCmdLists[c]->getd3d12GraphicsCommandList();

                // Perform swap after last graphics command list
                const bool swap = CommandQueueType::Graphics == cmdQueueType && queueCmdLists.size() == c + 1;

                if (swap)
                {
                    // Transition the swap chain back to present
                    D3D12_RESOURCE_BARRIER barrier;
                    barrier.Transition.pResource = m_bufferContext[curBackbuffer].backbuffer->getResource().getd3d12TextureResource();
                    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
                    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
                    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

                    d3d12cmdList->ResourceBarrier(1, &barrier);
                }

                d3d12cmdList->Close();
                cmdListsToExecute.push_back(d3d12cmdList);
            }

            if (cmdListsToExecute.size() > 0)
            {
                auto * d3d12queue = queue->getd3d12CommandQueue();
                d3d12queue->ExecuteCommandLists((uint)cmdListsToExecute.size(), cmdListsToExecute.data());
            }
        }

        const auto currentFrameIndex = (getFrameContextIndex() + 1) % max_frame_latency; // next frame
       
        for (uint q = 0; q < enumCount<CommandQueueType>(); ++q)
        {
            const auto cmdQueueType = (CommandQueueType)q;
            auto * queue = getCommandQueue(cmdQueueType);

            // TODO: separate fence for each queue ?
            if (CommandQueueType::Graphics == cmdQueueType)
            {
                auto * d3d12queue = queue->getd3d12CommandQueue();

                // Signal that the frame is complete
                auto & Frame = getCurrentFrameContext();
                #if GPU_FENCE_DEBUG
                VG_DEBUGPRINT("[Device::endFrame %u] Signal(%u)\n", m_frameCounter & 0xFF, Frame.mFrameFenceId);
                #endif
                VG_VERIFY_SUCCEEDED(m_d3d12fence->SetEventOnCompletion(Frame.mFrameFenceId, m_d3d12fenceEvent));
                VG_VERIFY_SUCCEEDED(d3d12queue->Signal(m_d3d12fence, Frame.mFrameFenceId));
            }
        }
        
        {
            VG_PROFILE_CPU("Present");
            VG_PROFILE_GPU_SWAP(this);
            VG_VERIFY_SUCCEEDED(m_dxgiSwapChain->Present((uint)m_VSync, 0));
        }

        if (m_HDRMode != m_HDRModeRequested)
        {
            waitGPUIdle(); 
            destroyd3d12Backbuffers();

            m_backbufferFormat = getHDRBackbufferFormat(m_HDRModeRequested);
            m_ColorSpace = getHDRColorSpace(m_HDRModeRequested);

            VG_VERIFY_SUCCEEDED(m_dxgiSwapChain->ResizeBuffers(max_backbuffer_count, m_dxgiSwapChainDesc.Width, m_dxgiSwapChainDesc.Height, Texture::getd3d12ResourceFormat(m_backbufferFormat), m_dxgiSwapChainDesc.Flags));
            created3d12Backbuffers();

            m_HDRMode = m_HDRModeRequested;

            if (HDR::None != m_HDRMode)
                VG_INFO("[Device] Use %s backbuffer", asString(m_HDRMode).c_str());
            else
                VG_INFO("[Device] Disable HDR backbuffer");
        }

        if (m_ColorSpace != m_ColorSpaceRequested)
        {
            applyColorSpace(m_ColorSpaceRequested);
            m_ColorSpace = m_ColorSpaceRequested;
        }

		super::endFrame();
	}
}