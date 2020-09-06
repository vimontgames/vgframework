#pragma once

namespace vg::graphics::driver
{
	constexpr auto max_frame_latency = 2;

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