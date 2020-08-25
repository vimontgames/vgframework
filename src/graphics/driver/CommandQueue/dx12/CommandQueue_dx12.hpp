namespace vg::graphics::driver::dx12
{
	//--------------------------------------------------------------------------------------
	CommandQueue::CommandQueue(CommandQueueType _type, core::uint _index) :
		base::CommandQueue(_type, _index)
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
								 desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
								 desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		auto * d3d12Device = driver::Device::get()->getd3d12Device();

		VG_ASSERT_SUCCEEDED(d3d12Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_d3d12commandQueue)));
	}

	//--------------------------------------------------------------------------------------
	CommandQueue::~CommandQueue()
	{
		VG_SAFE_RELEASE(m_d3d12commandQueue);
	}
}