#pragma once

namespace vg::gfx
{
	constexpr auto max_frame_latency = 2;       // The maximum number of frames that will be generated on the CPU before waiting for the earliest one to complete.
    constexpr auto max_backbuffer_count = 3;    // Can be different or equal than 'max_frame_latency' depending on the GPU/CPU sync mode 

    enum class Severity : core::u8
    {
        Error = 0,
        Warning,
        Info,
        Verbose
    };

    enum class API
    {
        DirectX12,
        Vulkan,
        Metal
    };

    enum class VSync : core::u8
    {
        None = 0,
        VBL_1,
        VBL_2,
        VBL_3,
        VBL_4
    };

    struct DeviceParams
    {
        core::WinHandle			window = core::invalidWindowHandle;
        core::InstanceHandle	instance = core::invalidInstanceHandle;
        API						api = API::DirectX12;
        core::uint2				resolution = { 1280,720 };
        bool					debugDevice = false;
        bool					breakOnErrors = false;
        bool					breakOnWarnings = false;
    };
}