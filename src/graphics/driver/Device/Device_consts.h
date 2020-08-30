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
}