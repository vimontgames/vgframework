#pragma once

namespace vg::gfx::vulkan
{
    //--------------------------------------------------------------------------------------
    // Vulkan-specific caps
    //--------------------------------------------------------------------------------------
    class DeviceCaps : public base::DeviceCaps
    {
    public:
        VG_CLASS_DECL(DeviceCaps, base::DeviceCaps);

        struct
        {
            u32 majorVersion = 0;
            u32 minorVersion = 0;
            u32 patchVersion = 0;
            u32 headerVersion = 0;

            bool deviceAddress = false;
            core::uint  rayTracingAccelerationStructureScratchOffsetAlignment = 0;
         } vulkan;
    };
}