#pragma once

namespace vg::graphics::driver
{
	enum class CommandListType : core::u8
	{
		Graphics = 0,
		Compute,
		Present
	};

#ifdef VG_DX12
	namespace dx12
	{
        using D3D12Device = ID3D12Device5;
		using D3D12GraphicsCommandList= ID3D12GraphicsCommandList4;
	}
#endif
}