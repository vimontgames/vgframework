#pragma once

#include VG_GFXAPI_HEADER(DeviceCaps)

namespace vg::graphics::driver
{
    //--------------------------------------------------------------------------------------
    // Cross-platform device caps + platform caps
    //--------------------------------------------------------------------------------------
    class DeviceCaps : public VG_GFXAPI::DeviceCaps
    {
    public:
        bool supportRaytracing = false;
    };
}