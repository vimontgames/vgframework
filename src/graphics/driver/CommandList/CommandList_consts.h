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
		using DirectX12GraphicsCommandList= ID3D12GraphicsCommandList4;
	}
#endif
}