#pragma once

#include "gfx/CommandList/CommandList_consts.h"

namespace vg::gfx
{
    class RootSignature;

    class PipelineStateCache
    {
    public:
        enum class DirtyFlags : core::u32
        {
            RootSignature   = 0x00000001,
            PipelineState   = 0x00000002,
            Viewport        = 0x00000004,
            Scissor         = 0x00000008,
            RootConstants   = 0x00000010,
            IndexBuffer     = 0x00000020,
            StencilRefValue = 0x00000040
        };

        PipelineStateCache() = default;

        void clear();
        void reset();

        DirtyFlags          m_dirtyFlags;
        core::u32           m_rootConstants[max_root_constants] = {};

        RootSignature *     m_rootSignature = nullptr;
    };
}