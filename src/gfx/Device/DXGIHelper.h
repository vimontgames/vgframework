#pragma once

#if VG_WINDOWS

struct IDXGIFactory6;

#include "gfx/Device/DeviceCaps.h"

namespace vg::gfx
{
    class DXGIHelper
    {
    public:
        static DeviceCaps::HDRCaps getHDRCaps(HWND _hwnd, IDXGIFactory6 * _dxgiFactory = nullptr);

    private:
        static int computeIntersectionArea(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2);
    };
}

#endif // VG_WINDOWS