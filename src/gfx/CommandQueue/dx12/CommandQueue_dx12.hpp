namespace vg::gfx::dx12
{
	//--------------------------------------------------------------------------------------
	CommandQueue::CommandQueue(CommandQueueType _type) :
		base::CommandQueue(_type)
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
								 desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
								 desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		auto * D3D12Device = gfx::Device::get()->getd3d12Device();
		VG_VERIFY_SUCCEEDED(D3D12Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_d3d12commandQueue)));
	}

	//--------------------------------------------------------------------------------------
	CommandQueue::~CommandQueue()
	{
		VG_SAFE_RELEASE(m_d3d12commandQueue);
	}
}