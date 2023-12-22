namespace vg::gfx::dx12
{
	//--------------------------------------------------------------------------------------
	CommandPool::CommandPool(uint _frame, uint _index) :
		super::CommandPool(_frame, _index)
	{
		auto * device = gfx::Device::get();
		VG_VERIFY_SUCCEEDED(device->getd3d12Device()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_d3d12cmdAllocator)));
	}

	//--------------------------------------------------------------------------------------
	CommandPool::~CommandPool()
	{
		VG_SAFE_RELEASE(m_d3d12cmdAllocator);
	}

	//--------------------------------------------------------------------------------------
	void CommandPool::beginFrame()
	{
		VG_VERIFY_SUCCEEDED(m_d3d12cmdAllocator->Reset());
	}
}