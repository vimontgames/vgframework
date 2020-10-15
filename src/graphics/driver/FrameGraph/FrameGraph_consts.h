#pragma once

namespace vg::graphics::driver
{
    constexpr core::uint maxSubPassPerRenderPass = 1;

    enum class ResourceState : core::u8
    {
        Undefined = 0,
        RenderTarget,
        ShaderResource
    };
}