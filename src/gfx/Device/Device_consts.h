#pragma once

vg_enum_class_ns(vg::gfx, Severity, core::u8,
    Error = 0,
    Warning,
    Info,
    Verbose
);

vg_enum_class_ns(vg::gfx, API, core::u8,
    DirectX12,
    Vulkan,
    Metal
);

vg_enum_class_ns(vg::gfx, VSync, core::u8,
    None = 0,
    VSync_1,
    VSync_2,
    VSync_3,
    VSync_4
);

vg_enum_class_ns(vg::gfx, AAPostProcess, core::u8,
    None = 0,
    FXAA,
    SMAA
);

vg_enum_class_ns(vg::gfx, HDR, core::u8,
    None = 0,
    HDR10 = 1,
    HDR16 = 2
);

vg_enum_class_ns(vg::gfx, HDRFlags, core::u8,
    None = 1 << (core::u8)HDR::None,
    HDR10 = 1 << (core::u8)HDR::HDR10,
    HDR16 = 1 << (core::u8)HDR::HDR16
);

vg_enum_class_ns(vg::gfx, ColorSpace, core::u8,
    Rec709 = 0,
    ST2084,
    Rec2020
);

namespace vg::gfx
{
	constexpr auto max_frame_latency = 2;       // The maximum number of frames that will be generated on the CPU before waiting for the earliest one to complete.
    constexpr auto max_backbuffer_count = 3;    // Can be different or equal than 'max_frame_latency' depending on the GPU/CPU sync mode 

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