namespace vg::graphics::driver::dx12
{
	//--------------------------------------------------------------------------------------
	void Device::init(const DeviceParams & _params)
	{
		super::init(_params);

        uint dxgiFactoryFlags = 0;
		if (_params.debugDevice)
		{
			VG_ASSERT_SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_d3d12debug)));

			if (m_d3d12debug)
				m_d3d12debug->EnableDebugLayer();

            dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG; 
		}
		VG_ASSERT_SUCCEEDED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_dxgiFactory)));

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
        D3D12Device * device = nullptr;

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
            HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

        D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {};
        if (!FAILED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5))))
        {
            m_caps.supportRaytracing = options5.RaytracingTier >= D3D12_RAYTRACING_TIER_1_0;
            m_caps.d3d12.raytracier_tier = options5.RaytracingTier;
        }

        m_memoryAllocator = new driver::MemoryAllocator();

		auto * graphicsQueue = createCommandQueue(CommandQueueType::Graphics);
		auto * swapChain = created3d12SwapChain((HWND)_params.window, _params.resolution.x, _params.resolution.y);

		// Create fences for each frame so we can protect resources and wait for any given frame
        m_nextFrameFence = 1;
        m_nextFrameIndex = 0;
        m_d3d12fenceEvent = CreateEvent(nullptr, false, false, nullptr);
        VG_ASSERT_SUCCEEDED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_d3d12fence)));

        // Rendertarget CPU descriptor heap
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = m_renderTargetDescriptorAllocated;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		VG_ASSERT_SUCCEEDED(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_renderTargetDescriptorHeap)));
		m_renderTargetDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		// For now just create command pools to handle latency
		// We'll need later to have one per thread recording command buffers
		for (uint i = 0; i < max_frame_latency; ++i)
			createFrameContext(i);

        for (uint i = 0; i < max_backbuffer_count; ++i)
        {
            ID3D12Resource * d3d12backbufferResource = nullptr;
            m_dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&d3d12backbufferResource));
            createBackbuffer(i, d3d12backbufferResource);
            d3d12backbufferResource->Release();
        }

        m_bindlessTable = new driver::BindlessTable();
	}

	//--------------------------------------------------------------------------------------
	IDXGISwapChain3 * Device::created3d12SwapChain(HWND _winHandle, core::uint _width, core::uint _height)
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
		swapChainDesc.BufferCount = max_backbuffer_count;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		//swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING /*| DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT*/;
        swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
		swapChainDesc.Flags |= fullScreen ? DXGI_SWAP_CHAIN_FLAG_FULLSCREEN_VIDEO : 0;

		auto * graphicsQueue = getCommandQueue(CommandQueueType::Graphics);

		VG_ASSERT_SUCCEEDED(m_dxgiFactory->CreateSwapChainForHwnd(graphicsQueue->getd3d12CommandQueue(), _winHandle, &swapChainDesc, nullptr, nullptr, (IDXGISwapChain1**)&m_dxgiSwapChain));
        
        VG_ASSERT_SUCCEEDED(m_dxgiFactory->MakeWindowAssociation(_winHandle, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_PRINT_SCREEN));

        m_currentBackbufferIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();

        if (swapChainDesc.Flags & DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT)
        {
            VG_ASSERT_SUCCEEDED(m_dxgiSwapChain->SetMaximumFrameLatency(max_frame_latency));
            //mSwapEvent = m_dxgiSwapChain->GetFrameLatencyWaitableObject();
        }

		return m_dxgiSwapChain;
	}

    //--------------------------------------------------------------------------------------
    //void WaitForFence(ID3D12Fence* fence, UINT64 completionValue, HANDLE waitEvent)
	//{
    //    #if VG_DBG_CPUGPUSYNC
    //    VG_DEBUGPRINT("Wait for fence %u ...\n", completionValue);
    //    #endif
    //
    //    const auto completed = fence->GetCompletedValue();
	//	if (completed < completionValue)
	//	{
    //        #if VG_DBG_CPUGPUSYNC
    //        VG_DEBUGPRINT("SetEventOnCompletion\n");
    //        #endif
    //
	//		VG_ASSERT_SUCCEEDED(fence->SetEventOnCompletion(completionValue, waitEvent));
	//		const auto result = WaitForSingleObject(waitEvent, INFINITE);
    //
    //        VG_ASSERT(WAIT_ABANDONED != result, "The specified object is a mutex object that was not released by the thread that owned the mutex object before the owning thread terminated. Ownership of the mutex object is granted to the calling thread and the mutex state is set to nonsignaled. If the mutex was protecting persistent state information, you should check it for consistency.");
    //        VG_ASSERT(WAIT_TIMEOUT != result, "The time-out interval elapsed, and the object's state is nonsignaled.");
    //        VG_ASSERT(WAIT_FAILED != result, "The function has failed. To get extended error information, call GetLastError.");
    //        VG_ASSERT(WAIT_OBJECT_0 == result, "The state of the specified should be signaled.");
	//	}
	//}

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
		VG_ASSERT(m_renderTargetDescriptorUsed + _count <= m_renderTargetDescriptorAllocated);
		VG_ASSERT(s_invalidRenderTargetDescriptorSize != m_renderTargetDescriptorSize);

		D3D12_CPU_DESCRIPTOR_HANDLE handle = { m_renderTargetDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
		handle.ptr += m_renderTargetDescriptorUsed * m_renderTargetDescriptorSize;
		m_renderTargetDescriptorUsed += _count;

		return handle;
	}

	//--------------------------------------------------------------------------------------
	void Device::freeRTVHandle(D3D12_CPU_DESCRIPTOR_HANDLE & _hRTV)
	{
		// TODO
	}

    ////--------------------------------------------------------------------------------------
    //D3D12_CPU_DESCRIPTOR_HANDLE Device::allocSRVHandle(core::uint _count)
    //{
    //    VG_ASSERT(m_srvDescriptorUsed + _count <= m_renderTargetDescriptorAllocated);
    //    VG_ASSERT(s_invalidSrvDescriptorSize != m_srcDescriptorHeapSize);
    //
    //    D3D12_CPU_DESCRIPTOR_HANDLE handle = { m_srvCPUDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
    //    handle.ptr += m_srvDescriptorUsed * m_srcDescriptorHeapSize;
    //    m_srvDescriptorUsed += _count;
    //
    //    return handle;
    //}
    //
    ////--------------------------------------------------------------------------------------
    //void Device::freeSRVHandle(D3D12_CPU_DESCRIPTOR_HANDLE & _hSRV)
    //{
    //    // TODO
    //}

    //--------------------------------------------------------------------------------------
    // Wait for everything to finish
    //--------------------------------------------------------------------------------------
    void Device::waitGPUIdle()
    {
        //for (int i = 0; i < max_frame_latency; ++i)
        //    WaitForFence(m_frameFences[i], m_fenceValues[i], m_frameFenceEvents);   

        for (uint q = 0; q < enumCount<CommandQueueType>(); ++q)
        {
            const auto cmdQueueType = (CommandQueueType)q;
            auto * queue = getCommandQueue(cmdQueueType);

            // TODO: separate fence for each queue ?
            if (CommandQueueType::Graphics == cmdQueueType)
            {
                auto * d3d12queue = queue->getd3d12CommandQueue();

                static UINT64 exit_fence = 0;
                d3d12queue->Signal(m_d3d12fence, exit_fence);
                m_d3d12fence->SetEventOnCompletion(exit_fence, m_d3d12fenceEvent);
                WaitForSingleObject(m_d3d12fenceEvent, INFINITE);
            }
        }
    }

	//--------------------------------------------------------------------------------------
	void Device::deinit()
	{
        for (uint i = 0; i < countof(m_frameContext); ++i)
            destroyFrameContext(i);

        for (uint i = 0; i < countof(m_bufferContext); ++i)
            destroyBackbuffer(i);

		VG_SAFE_RELEASE(m_renderTargetDescriptorHeap);

        //for (uint i = 0; i < max_frame_latency; ++i)
        //	VG_SAFE_RELEASE(m_frameFences[i]);

        VG_SAFE_RELEASE(m_d3d12fence);

		VG_SAFE_RELEASE(m_dxgiSwapChain);

		destroyCommandQueues();
		
        VG_SAFE_DELETE(m_bindlessTable);

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
            debugInterface->SetEnableSynchronizedCommandQueueValidation(false);
			debugInterface->Release();
		}
		#endif

		super::deinit();
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
        FrameContext * Frame = &m_frameContext[FrameIndex];
        WaitForFence(m_d3d12fence, m_d3d12fenceEvent, Frame->mFrameFenceId);
        Frame->mFrameFenceId = FrameFence;
        m_currentFrameIndex = FrameIndex;

		auto & context = getCurrentFrameContext();

		for (auto & cmdPool : context.commandPools)
			cmdPool->beginFrame();

		for (uint type = 0; type < enumCount<CommandListType>(); ++type)
			for (auto & cmdList : context.commandLists[type])
				cmdList->reset();

        m_bindlessTable->beginFrame();

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
                VG_ASSERT_SUCCEEDED(m_d3d12fence->SetEventOnCompletion(Frame.mFrameFenceId, m_d3d12fenceEvent));
                VG_ASSERT_SUCCEEDED(d3d12queue->Signal(m_d3d12fence, Frame.mFrameFenceId));
            }
        }

        VG_ASSERT_SUCCEEDED(m_dxgiSwapChain->Present(0, 0));

		super::endFrame();
	}
}