namespace vg::graphics::driver::dx12
{
	//--------------------------------------------------------------------------------------
	CommandPool::CommandPool(uint _frame, uint _index) :
		super::CommandPool(_frame, _index)
	{
		auto * device = driver::Device::get();
		VG_ASSERT_SUCCEEDED(device->getd3d12Device()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_d3d12cmdAllocator)));
	}

	//--------------------------------------------------------------------------------------
	CommandPool::~CommandPool()
	{
		VG_SAFE_RELEASE(m_d3d12cmdAllocator);
	}

	//--------------------------------------------------------------------------------------
	void CommandPool::beginFrame()
	{
		VG_ASSERT_SUCCEEDED(m_d3d12cmdAllocator->Reset());
	}
}