#pragma once

namespace vg::gfx
{
    constexpr core::uint maxSubPassPerRenderPass = 1;

    using FrameGraphUserPassID = core::string;
    using FrameGraphResourceID = core::string;

    vg_enum_class(vg::gfx, ResourceState, core::u8,
        Undefined = 0,
        RenderTarget,
        ShaderResource,
        UnorderedAccess,
        NonPixelShaderResource
    );

    vg_enum_class(vg::gfx, RenderPassType, core::u8,
        Graphic = 0,
        Compute,
        Copy,
        RayTrace
    );

    vg_enum_class(vg::gfx, RWFlags, core::u8,
        Read    = 0x01,
        Write   = 0x02
    );

    vg_enum_class(vg::gfx, RenderJobsPolicy, core::u8,
        MainThread = 0,
        NodeCount,
        TargetCost,
        RecursiveSplit,
        CumulativeCost
    );
}