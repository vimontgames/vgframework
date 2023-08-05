#pragma once

namespace vg::gfx::dx12
{
    //--------------------------------------------------------------------------------------
    // DX12-specific caps
    //--------------------------------------------------------------------------------------
    class DeviceCaps
    {
    public:
        struct
        {
            D3D12_RAYTRACING_TIER raytracier_tier = D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
        } d3d12;
    };
}