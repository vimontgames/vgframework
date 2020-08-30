namespace vg::graphics::driver::dx12
{
	//--------------------------------------------------------------------------------------
	void Device::init(const DeviceParams & _params)
	{
		super::init(_params);

		if (_params.debugDevice)
		{
			VG_ASSERT_SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_d3d12debug)));

			if (m_d3d12debug)
				m_d3d12debug->EnableDebugLayer();
		}

		VG_ASSERT_SUCCEEDED(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_dxgiFactory)));

		const D3D_FEATURE_LEVEL levels[] =
		{
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
		ID3D12Device * device = nullptr;	

		for (uint l = 0; l < countof(levels); ++l)
		{
			for (u32 a = 0; !m_d3d12device && DXGI_ERROR_NOT_FOUND != m_dxgiFactory->EnumAdapters1(a, &adapter); ++a)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				HRESULT hr = D3D12CreateDevice(adapter, levels[l], IID_PPV_ARGS(&device));
				if (SUCCEEDED(hr))
				{
					adapter->GetDesc1(&desc);

					m_d3d12device = device;
                    m_dxgiAdapter = adapter;
					m_level = levels[l];
					
					uint major = (m_level & 0xF000) >> 12;
					uint minor = (m_level & 0x0F00) >> 8;

					char description[256];
					size_t s;
					wcstombs_s(&s, description, countof(description), desc.Description, wcslen(desc.Description));

					VG_DEBUGPRINT("Device : DirectX %u.%u %s\n", major, minor, m_d3d12debug ? "debug " : "");
                    VG_DEBUGPRINT("Adapter: %s\n", description);
				}
			}
		}
		VG_ASSERT(device, "Could not create DirectX device\n");

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

        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
        auto HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
        if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
        {
            HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        m_memoryAllocator = new driver::MemoryAllocator();

		auto * graphicsQueue = createCommandQueue(CommandQueueType::Graphics);
		auto * swapChain = created3d12SwapChain((HWND)_params.window, _params.resolution.x, _params.resolution.y);

		// Create fences for each frame so we can protect resources and wait for any given frame
		m_currentFenceValue = 1;
	
		for (int i = 0; i < max_frame_latency; ++i)
		{
			m_frameFenceEvents[i] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			m_fenceValues[i] = 0;
			device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_frameFences[i]));
		}

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = m_renderTargetDescriptorAllocated;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_renderTargetDescriptorHeap));

		m_renderTargetDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		// For now just create command pools to handle latency
		// We'll need later to have one per thread recording command buffers
		for (uint i = 0; i < max_frame_latency; ++i)
		{
			ID3D12Resource * d3d12backbufferResource = nullptr;
			m_dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&d3d12backbufferResource));

			createFrameContext(i, d3d12backbufferResource);

			d3d12backbufferResource->Release();
		}
	}

	//--------------------------------------------------------------------------------------
	IDXGISwapChain1 * Device::created3d12SwapChain(HWND _winHandle, core::uint _width, core::uint _height)
	{
		VG_ASSERT(core::invalidWindowHandle != _winHandle);

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
		::ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

        m_backbufferFormat = PixelFormat::R8G8B8A8_unorm;

		const bool fullScreen = false; // TODO

		swapChainDesc.Width = _width;
		swapChainDesc.Height = _height;
		swapChainDesc.Format = Texture::getd3d12PixelFormat(m_backbufferFormat);
		swapChainDesc.Stereo = false;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = max_frame_latency;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapChainDesc.Flags |= fullScreen ? DXGI_SWAP_CHAIN_FLAG_FULLSCREEN_VIDEO : 0;

		auto * graphicsQueue = getCommandQueues(CommandQueueType::Graphics)[0];

		VG_ASSERT_SUCCEEDED(m_dxgiFactory->CreateSwapChainForHwnd(graphicsQueue->getd3d12CommandQueue(), _winHandle, &swapChainDesc, nullptr, nullptr, &m_dxgiSwapChain));
		return m_dxgiSwapChain;
	}

    //--------------------------------------------------------------------------------------
    void WaitForFence(ID3D12Fence* fence, UINT64 completionValue, HANDLE waitEvent)
	{
		if (fence->GetCompletedValue() < completionValue)
		{
			fence->SetEventOnCompletion(completionValue, waitEvent);
			WaitForSingleObject(waitEvent, INFINITE);
		}
	}

	//--------------------------------------------------------------------------------------
	D3D12_CPU_DESCRIPTOR_HANDLE Device::allocRenderTargetViewHandle(core::uint _count)
	{
		VG_ASSERT(m_renderTargetDescriptorUsed + _count <= m_renderTargetDescriptorAllocated);
		VG_ASSERT(s_invalidRenderTargetDescriptorSize != m_renderTargetDescriptorSize);

		D3D12_CPU_DESCRIPTOR_HANDLE handle = { m_renderTargetDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
		handle.ptr += m_renderTargetDescriptorUsed * m_renderTargetDescriptorSize;
		m_renderTargetDescriptorUsed += _count;

		return handle;
	}

	//--------------------------------------------------------------------------------------
	void Device::freeRenderTargetViewHandle(D3D12_CPU_DESCRIPTOR_HANDLE & _hRTV)
	{
		// TODO
	}

	//--------------------------------------------------------------------------------------
	void Device::deinit()
	{
		// Drain the queue, wait for everything to finish
		for (int i = 0; i < max_frame_latency; ++i)
			WaitForFence(m_frameFences[i], m_fenceValues[i], m_frameFenceEvents[i]);

		for (uint i = 0; i < countof(m_frameContext); ++i)
			destroyFrameContext(i);

		VG_SAFE_RELEASE(m_renderTargetDescriptorHeap);

		for (uint i = 0; i < max_frame_latency; ++i)
			VG_SAFE_RELEASE(m_frameFences[i]);

		VG_SAFE_RELEASE(m_dxgiSwapChain);

		destroyCommandQueues();
		
		VG_SAFE_RELEASE(m_dxgiFactory);
		VG_SAFE_RELEASE(m_d3d12debug);
		VG_SAFE_RELEASE(m_dxgiAdapter);
        VG_SAFE_RELEASE(m_memoryAllocator);
		VG_SAFE_RELEASE(m_d3d12device);

		#if 1
		IDXGIDebug1 * debug = nullptr;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
		{
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL);
			debug->Release();
		}
		#else
		ID3D12DebugDevice * debugInterface;
		if (SUCCEEDED(m_d3d12device->QueryInterface(&debugInterface)))
		{
			debugInterface->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
			debugInterface->Release();
		}
		#endif

		super::deinit();
	}

	//--------------------------------------------------------------------------------------
	ID3D12Device * Device::getd3d12Device() const
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

        const auto currentFrameIndex = getFrameContextIndex();

        if (m_fenceValues[currentFrameIndex])
        {
            VG_DEBUGPRINT("Wait completion of frame %u (fence[%u] = %u)\n", m_frameCounter - max_frame_latency, currentFrameIndex, m_fenceValues[currentFrameIndex]);
            WaitForFence(m_frameFences[currentFrameIndex], m_fenceValues[currentFrameIndex], m_frameFenceEvents[currentFrameIndex]);
        }

		auto & context = getCurrentFrameContext();

		for (auto & cmdPool : context.commandPools)
			cmdPool->beginFrame();

		for (uint type = 0; type < enumCount<CommandListType>(); ++type)
			for (auto & cmdList : context.commandLists[type])
				cmdList->reset();

		auto * commandList = context.commandLists[asInteger(CommandListType::Graphics)][0]->getd3d12GraphicsCommandList();

		// Transition back buffer
		D3D12_RESOURCE_BARRIER barrier;
		barrier.Transition.pResource = m_frameContext[currentFrameIndex].backbuffer->getd3d12Resource();
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
		auto * queue = getCommandQueues(CommandQueueType::Graphics)[0]->getd3d12CommandQueue();

		auto & context = getCurrentFrameContext();

		// Transition the swap chain back to present
		D3D12_RESOURCE_BARRIER barrier;
		barrier.Transition.pResource = context.backbuffer->getd3d12Resource();
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		auto commandList = context.commandLists[asInteger(CommandListType::Graphics)][0]->getd3d12GraphicsCommandList();
		commandList->ResourceBarrier(1, &barrier);

		commandList->Close();

		// Execute our commands
		ID3D12CommandList* commandLists[] = { commandList };
		queue->ExecuteCommandLists(std::extent<decltype(commandLists)>::value, commandLists);

		m_dxgiSwapChain->Present(1, 0);

		// Mark the fence for the current frame.
		const auto fenceValue = m_currentFenceValue;
        const auto currentFrameIndex = getFrameContextIndex();

        VG_DEBUGPRINT("Write fence %u (fence[%u] = %u)\n", currentFrameIndex, currentFrameIndex, fenceValue);

		queue->Signal(m_frameFences[currentFrameIndex], fenceValue);
		m_fenceValues[currentFrameIndex] = fenceValue;
		++m_currentFenceValue;

		super::endFrame();
	}
}