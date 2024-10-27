#pragma once

namespace vg::gfx::vulkan
{
    //--------------------------------------------------------------------------------------
    // Vulkan-specific caps
    //--------------------------------------------------------------------------------------
    class DeviceCaps : public base::DeviceCaps
    {
    public:
        bool        deviceAddress = false;
        core::uint  rayTracingAccelerationStructureScratchOffsetAlignment = 0;
    };
}