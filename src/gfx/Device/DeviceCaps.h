#pragma once

#include VG_GFXAPI_HEADER(DeviceCaps)

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    // Cross-platform device caps + platform caps
    //--------------------------------------------------------------------------------------
    class DeviceCaps : public VG_GFXAPI::DeviceCaps
    {
    public:
		void init();

        bool supportRaytracing = false;
    };
}