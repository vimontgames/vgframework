#pragma once

namespace vg::gfx
{
    constexpr core::uint maxSubPassPerRenderPass = 1;

    enum class ResourceState : core::u8
    {
        Undefined = 0,
        RenderTarget,
        ShaderResource,
        UnorderedAccess
    };

    enum class RenderPassType : core::u8
    {
        Graphic = 0,
        Compute,
        Copy,
        RayTrace
    };

    enum class RWFlags : core::u8
    {
        Read    = 0x01,
        Write   = 0x02
    };
}