#pragma once

namespace vg::gfx
{
    // Vulkan uses DXC to compile shader and thus requires a ShaderLevel string
    // This is our cross-platform equivalent of D3D_SHADER_MODEL
    vg_enum_class(vg::gfx, ShaderModel, core::u8,
        SM_5_1 = 0x51,
        SM_6_0 = 0x60,
        SM_6_1 = 0x61,
        SM_6_2 = 0x62,
        SM_6_3 = 0x63,
        SM_6_4 = 0x64,
        SM_6_5 = 0x65,
        SM_6_6 = 0x66,
        SM_6_7 = 0x67
    );
}

namespace vg::gfx::base
{
    class DeviceCaps : public core::Object
    {
    public:
        VG_CLASS_DECL(DeviceCaps, core::Object);
    };
}

#ifdef VG_DX12
#include "dx12/DeviceCaps_dx12.h"
#elif defined(VG_VULKAN)
#include "vulkan/DeviceCaps_vulkan.h"
#else
#error Undefined GFXAPI
#endif

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    // Cross-platform device caps + platform caps
    //--------------------------------------------------------------------------------------
    class DeviceCaps : public VG_GFXAPI::DeviceCaps
    {
    public:
        VG_CLASS_DECL(DeviceCaps, VG_GFXAPI::DeviceCaps);

        DeviceCaps(const core::string & _name = "DeviceCaps", core::IObject * _parent = nullptr);

		void init();

        core::string    gpuName;

        ShaderModel     shaderModel = (ShaderModel)0;

        // Memory
        struct MemoryCaps
        {
            core::uint dedicated = 0;
            core::uint system = 0;
            core::uint shared = 0;
        };
        MemoryCaps memory;

        // RayTracing
        struct RayTracingCaps
        {
            bool supported = false;
        };
        RayTracingCaps rayTracing;

        // MSAA
        struct MSAACaps
        {
            bool isSupported(MSAA _mode) const { return 0 != ((1 << core::uint(_mode)) & (uint)modes); }
            MSAAFlags modes = MSAAFlags::None;
        };
        MSAACaps msaa;

        // HDR
        struct HDRCaps
        {
            bool operator != (const HDRCaps & _other)
            {
                return modes != _other.modes || minLuminance != _other.minLuminance || maxLuminance != _other.maxLuminance || enableST2084 != _other.enableST2084 || referenceWhiteNits != _other.referenceWhiteNits;
            }

            bool isSupported(HDR _mode) const { return 0 != ((1<<core::uint(_mode)) & (uint)modes); }

            HDRFlags        modes = HDRFlags::None;
            float           minLuminance = 0.0f;
            float           maxLuminance = 400.0f;
            bool            enableST2084 = false;
            float           referenceWhiteNits = 80.0f;    // The reference brightness level of the display.
        };
        HDRCaps         hdr;       
    };
}