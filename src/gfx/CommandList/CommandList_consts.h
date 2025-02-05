#pragma once

#include "shaders/system/shared_consts.hlsli"

namespace vg::gfx
{
	vg_enum_class(vg::gfx, CommandListType, core::u8,
		Graphics = 0,
		Compute,
		Present
	);

    vg_enum_class(vg::gfx, PrimitiveType, core::u8,
        Point = 0,
        Line,
        Triangle
    );

    vg_enum_class(vg::gfx, PrimitiveTopology, core::u8,
        PointList = 0,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip
    );

    constexpr core::uint max_root_constants = 32;
    constexpr core::uint max_constant_buffers = 1;

    const core::float4 defaultOptimizedClearColor = (core::float4)0.0f;

    #if VG_GFX_REVERSE_DEPTH
    const float        defaultOptimizedClearDepth = 0.0f;
    #else 
    const float        defaultOptimizedClearDepth = 1.0f;
    #endif

    const core::u8     defaultOptimizedClearStencil = 0x00;

    #ifdef VG_DX12
    namespace dx12
    {
        using D3D12Device = ID3D12Device5;
        using D3D12GraphicsCommandList = ID3D12GraphicsCommandList4;
        using D3D12DXGISwapChain = IDXGISwapChain4;
        using D3D12DXGIFactory = IDXGIFactory6;
    }
    #endif
}