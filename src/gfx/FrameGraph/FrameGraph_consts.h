#pragma once

namespace vg::gfx
{
    constexpr core::uint maxSubPassPerRenderPass = 1;

    enum class ResourceState : core::u8
    {
        Undefined = 0,
        RenderTarget,
        ShaderResource
    };

    enum class RenderPassType : core::u8
    {
        Graphic = 0,
        Compute,
        RayTrace
    };
}