#pragma once

namespace vg::gfx::dx12
{
    //--------------------------------------------------------------------------------------
    // DX12-specific caps
    //--------------------------------------------------------------------------------------
    class DeviceCaps : public base::DeviceCaps
    {
    public:
        VG_CLASS_DECL(DeviceCaps, base::DeviceCaps);

        struct
        {
            D3D_FEATURE_LEVEL           featureLevel         = (D3D_FEATURE_LEVEL)0;
            core::uint                  headerVersion        = 0;
            D3D_SHADER_MODEL            shaderModel          = (D3D_SHADER_MODEL)0;
            D3D_ROOT_SIGNATURE_VERSION  rootSignatureVersion = (D3D_ROOT_SIGNATURE_VERSION)0;
            D3D12_RAYTRACING_TIER       raytracier_tier      = (D3D12_RAYTRACING_TIER)0;

        } d3d12;
    };
}