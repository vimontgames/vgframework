#pragma once

namespace vg::graphics::driver
{
	enum class CommandListType : core::u8
	{
		Graphics = 0,
		Compute,
		Present
	};

    enum class PrimitiveType : core::u8
    {
        Point = 0,
        Line,
        Triangle
    };

    enum class PrimitiveTopology : core::u8
    {
        PointList = 0,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip
    };

    constexpr core::uint max_root_constants = 32;
    constexpr core::uint max_constant_buffers = 1;

    #ifdef VG_DX12
    namespace dx12
    {
        using D3D12Device = ID3D12Device5;
        using D3D12GraphicsCommandList = ID3D12GraphicsCommandList4;
    }
    #endif
}