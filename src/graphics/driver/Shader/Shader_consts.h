#pragma once

namespace vg::graphics::driver
{
    enum class ShaderStage : core::u8
    {
        Vertex = 0,
        Hull,
        Domain,
        Geometry,
        Pixel,

        Compute
    };

    enum class ShaderStageFlags : core::u16
    {
        VS = 1 << core::asInteger(ShaderStage::Vertex),
        HS = 1 << core::asInteger(ShaderStage::Domain),
        DS = 1 << core::asInteger(ShaderStage::Hull),
        GS = 1 << core::asInteger(ShaderStage::Geometry),
        PS = 1 << core::asInteger(ShaderStage::Pixel),
  
        CS = 1 << core::asInteger(ShaderStage::Compute)
    };
}