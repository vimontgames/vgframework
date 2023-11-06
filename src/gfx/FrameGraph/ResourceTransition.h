#pragma once

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    // Flags used in ResourceTransitionDesc to describe a resource transition
    //--------------------------------------------------------------------------------------
    enum class ResourceTransitionFlags : core::u8
    {
        Discard         = 0x00,
        Clear           = 0x01, // clear before use
        Preserve        = 0x02, // init with previous contents of attachment

        RenderTarget    = 0x04, // attachment will be used as a render target during this pass
        Present         = 0x08  // attachment will be used for 'Present' right after this pass
    };

    //--------------------------------------------------------------------------------------
    // Structure used to describe a resource transition : 
    // It is used as part of RenderPass/SubPass key but also for other resource transitions 
    // generated during FrameGraph build.
    //--------------------------------------------------------------------------------------
    struct ResourceTransitionDesc
    {
        ResourceTransitionDesc(ResourceTransitionFlags _flags = ResourceTransitionFlags::Discard, ResourceState _begin = ResourceState::Undefined, ResourceState _end = ResourceState::Undefined) :
            flags(_flags),
            begin(_begin),
            end(_end)
        {

        }

        union
        {
            struct
            {
                ResourceTransitionFlags flags : 8;
                ResourceState           begin : 4;
                ResourceState           end   : 4;
            };
            core::u16 bits;
        };
    };

    VG_STATIC_ASSERT(sizeof(ResourceTransitionDesc) == sizeof(core::u16), "Invalid sizeof(ResourceTransitionDesc)");
}