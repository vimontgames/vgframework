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

        bool        deviceAddress = false;
        core::uint  rayTracingAccelerationStructureScratchOffsetAlignment = 0;
    };
}