#pragma once

namespace vg::gfx
{
	constexpr auto max_frame_latency = 2;       // The maximum number of frames that will be generated on the CPU before waiting for the earliest one to complete.
    constexpr auto max_backbuffer_count = 3;    // Can be different or equal than 'max_frame_latency' depending on the GPU/CPU sync mode 

    vg_enum_class(Severity, core::u8,
        Error = 0,
        Warning,
        Info,
        Verbose
    );

    vg_enum_class(API, core::u8,
        DirectX12,
        Vulkan,
        Metal
    );

    vg_enum_class(VSync, core::u8,
        None = 0,
        VSync_1,
        VSync_2,
        VSync_3,
        VSync_4
    );

    vg_enum_class(MSAA, core::u8,
        None    = 0x01,
        MSAA2X  = 0x02,
        MSAA4X  = 0x04,
        MSAA8X  = 0x08,
        MSAA16X = 0x10
    );

    vg_enum_class(AAPostProcess, core::u8,
        None = 0,
        FXAA,
        SMAA
    );

    vg_enum_class(HDR, core::u8,
        None    = 0x01,
        HDR10   = 0x0a,
        HDR16   = 0x10
    );

    vg_enum_class(ColorSpace, core::u8,
        Rec709 = 0,
        ST2084,
        Rec2020
    );

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