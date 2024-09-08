#pragma once

namespace vg::gfx
{
    // Vulkan uses DXC to compile shader and thus requires a ShaderLevel string
    // This is our cross-platform equivalent of D3D_SHADER_MODEL
    vg_enum_class(ShaderModel, core::u8,
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

#include VG_GFXAPI_HEADER(DeviceCaps)

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    // Cross-platform device caps + platform caps
    //--------------------------------------------------------------------------------------
    class DeviceCaps : public VG_GFXAPI::DeviceCaps
    {
    public:
        DeviceCaps();

		void init();

        ShaderModel     shaderModel = (ShaderModel)0;

        // RayTracing
        bool            rayTracing = false;
        core::uint      rayTracingAccelerationStructureScratchOffsetAlignment = 0;

        // HDR
        struct HDRCaps
        {
            HDRCaps()
            {
                for (auto i = 0; i < core::enumCount<HDR>(); ++i)
                    mode[i] = false;
                mode[core::asInteger(HDR::None)] = true;
            }

            bool operator != (const HDRCaps & _other)
            {
                for (auto i = 0; i < core::enumCount<HDR>(); ++i)
                {
                    if (mode[i] != _other.mode[i])
                        return true;
                }

                return minLuminance != _other.minLuminance || maxLuminance != _other.maxLuminance || enableST2084 != _other.enableST2084 || referenceWhiteNits != _other.referenceWhiteNits;
            }

            bool isSupported(HDR _mode) const { return mode[core::asInteger(_mode)]; }

            bool            mode[core::enumCount<HDR>()];
            float           minLuminance = 0.0f;
            float           maxLuminance = 400.0f;
            bool            enableST2084 = false;
            float           referenceWhiteNits = 80.0f;    // The reference brightness level of the display.
        };
        HDRCaps         hdr;       
    };
}