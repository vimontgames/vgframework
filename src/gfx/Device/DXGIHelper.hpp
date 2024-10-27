#if VG_WINDOWS
#include "DXGIHelper.h"
#include "dxgi1_6.h"

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    // https://learn.microsoft.com/en-us/samples/microsoft/directx-graphics-samples/d3d12-hdr-sample-win32/
    //--------------------------------------------------------------------------------------
    DeviceCaps::HDRCaps DXGIHelper::getHDRCaps(HWND _hwnd, IDXGIFactory6 * _dxgiFactory)
    {
        DeviceCaps::HDRCaps hdrCaps = {};

        bool releaseDXGIFactory = false;
        if (nullptr == _dxgiFactory || !_dxgiFactory->IsCurrent())
        {
            VG_VERIFY_SUCCEEDED(CreateDXGIFactory2(0, IID_PPV_ARGS(&_dxgiFactory)));
            releaseDXGIFactory = true;
        }

        IDXGIAdapter1 * adapter;
        VG_VERIFY_SUCCEEDED(_dxgiFactory->EnumAdapters1(0, &adapter));

        UINT i = 0;
        IDXGIOutput * currentOutput;
        IDXGIOutput * bestOutput = nullptr;
        float bestIntersectArea = -1;

        RECT bounds = {};
        GetWindowRect(_hwnd, &bounds);

        while (adapter->EnumOutputs(i, &currentOutput) != DXGI_ERROR_NOT_FOUND)
        {
            // Get the rectangle bounds of the window
            int ax1 = bounds.left;
            int ay1 = bounds.top;
            int ax2 = bounds.right;
            int ay2 = bounds.bottom;

            // Get the rectangle bounds of current output
            DXGI_OUTPUT_DESC desc;
            VG_VERIFY_SUCCEEDED(currentOutput->GetDesc(&desc));
            RECT r = desc.DesktopCoordinates;
            int bx1 = r.left;
            int by1 = r.top;
            int bx2 = r.right;
            int by2 = r.bottom;

            // Compute the intersection
            int intersectArea = computeIntersectionArea(ax1, ay1, ax2, ay2, bx1, by1, bx2, by2);
            if (intersectArea > bestIntersectArea)
            {
                if (bestOutput)
                    bestOutput->Release();

                bestOutput = currentOutput;
                bestIntersectArea = static_cast<float>(intersectArea);
            }
            else
                currentOutput->Release();

            i++;
        }

        bool hdrSupport = false;

        // Having determined the output (display) upon which the app is primarily being 
        // rendered, retrieve the HDR capabilities of that display by checking the color space.
        if (bestOutput)
        {
            IDXGIOutput6 * output6 = nullptr;
            HRESULT hr = bestOutput->QueryInterface(__uuidof(IDXGIOutput6), reinterpret_cast<void **>(&output6));
            if (SUCCEEDED(hr))
            {
                DXGI_OUTPUT_DESC1 desc1 = {};
                VG_VERIFY_SUCCEEDED(output6->GetDesc1(&desc1));

                hdrSupport = (desc1.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);

                if (hdrSupport)
                {
                    hdrCaps.minLuminance = desc1.MinLuminance;
                    hdrCaps.maxLuminance = desc1.MaxLuminance;
                }

                output6->Release();
            }
            else
            {
                VG_ASSERT(false, "IDXGIOutput6 not be supported");
            }

            bestOutput->Release();
        }

        hdrCaps.modes |= HDR::None;

        if (hdrSupport)
        {
            // How to differentiate HDR10 support from HDR16 support?
            hdrCaps.modes |= HDR::HDR10;
            hdrCaps.modes |= HDR::HDR16;
        }

        adapter->Release();

        if (releaseDXGIFactory)
            _dxgiFactory->Release();

        return hdrCaps;
    }

    //--------------------------------------------------------------------------------------
    // To detect HDR support, we will need to check the color space in the primary DXGI output associated with the app at
    // this point in time (using window/display intersection). 
    // Compute the overlay area of two rectangles, A and B.
    // (ax1, ay1) = left-top coordinates of A; (ax2, ay2) = right-bottom coordinates of A
    // (bx1, by1) = left-top coordinates of B; (bx2, by2) = right-bottom coordinates of B
    //--------------------------------------------------------------------------------------
    int DXGIHelper::computeIntersectionArea(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2)
    {
        return max(0, min(ax2, bx2) - max(ax1, bx1)) * max(0, min(ay2, by2) - max(ay1, by1));
    }
}
#endif // VG_WINDOWS