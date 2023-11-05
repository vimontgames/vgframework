#pragma once

#include "FrameGraph_consts.h"
#include "gfx/Resource/Texture_consts.h"
#include "gfx/PipelineState/Graphic/GraphicPipelineState_consts.h"
#include "ResourceTransition.h"

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    // Descriptor for a subPass used in RenderPass
    // bits [0..31] used for up to maxRenderTarget (8) render targets
    // bits [32..35] used for depthStencil
    // bits [36..63] padding
    //--------------------------------------------------------------------------------------
    struct SubPassKey
    {
        ResourceTransitionDesc color[maxRenderTarget];
        ResourceTransitionDesc depth;

        inline void setColorAttachmentInfo(core::uint _index, const ResourceTransitionDesc & _attachmentInfos)
        {
            VG_ASSERT(_index < maxRenderTarget);
            color[_index] = _attachmentInfos;
        }

        inline const ResourceTransitionDesc & getColorAttachmentInfo(core::uint _index) const
        {
            VG_ASSERT(_index < maxRenderTarget);
            return color[_index];
        }

        inline void setDepthStencilAttachmentInfo(const ResourceTransitionDesc & _attachmentInfos)
        {
            depth = _attachmentInfos;
        }

        inline const ResourceTransitionDesc & getDepthStencilAttachmentInfo() const
        {
            return depth;
        }

        inline bool operator == (const SubPassKey & _other) const
        {
            for (core::uint i = 0; i < maxRenderTarget; ++i)
                if (color[i].bits != _other.color[i].bits)
                    return false;

            return _other.depth.bits == depth.bits;
        }

        inline bool operator != (const SubPassKey & _other) const
        {
            return !(_other == *this);
        }

        struct hash
        {
            inline core::size_t operator() (const SubPassKey & _this) const
            {
                auto ret = core::hash<core::u8>()(core::u8(_this.depth.bits));
                for (core::uint i = 0; i < maxRenderTarget; ++i)
                    ret ^= core::hash<core::u8>()(core::u8(_this.color[i].bits));
                return ret;
            }
        };
    };

    //--------------------------------------------------------------------------------------
    // Descriptor for a RenderPass and its subPasses
    //--------------------------------------------------------------------------------------
    struct RenderPassKey
    {
        RenderPassKey() :
            m_depthStencilFormat(PixelFormat::Unknow),
            m_subPassCount(0)
        {
            for (core::uint i = 0; i < maxRenderTarget; ++i)
                m_colorFormat[i] = PixelFormat::Unknow;
        }

        PixelFormat m_colorFormat[maxRenderTarget];
        PixelFormat m_depthStencilFormat;
        SubPassKey  m_subPassKeys[maxSubPassPerRenderPass];
        core::u8    m_subPassCount;

        inline bool operator == (const RenderPassKey & _other) const
        {
            for (core::uint i = 0; i < maxRenderTarget; ++i)
                if (_other.m_colorFormat[i] != m_colorFormat[i])
                    return false;

            for (core::uint i = 0; i < maxSubPassPerRenderPass; ++i)
                if (_other.m_subPassKeys[i] != m_subPassKeys[i])
                    return false;

            return _other.m_depthStencilFormat == m_depthStencilFormat && _other.m_subPassCount == m_subPassCount;
        }

        struct hash
        {
            inline core::size_t operator() (const RenderPassKey & _this) const
            {
                core::size_t ret = 0;

                for (core::uint i = 0; i < maxRenderTarget; ++i)
                    ret ^= core::hash<std::underlying_type<PixelFormat>::type>()(std::underlying_type<PixelFormat>::type(_this.m_colorFormat[i]));

                ret ^= core::hash<core::u8>()((core::u8)_this.m_depthStencilFormat);

                for (core::uint i = 0; i < maxSubPassPerRenderPass; ++i)
                    ret ^= SubPassKey::hash()(_this.m_subPassKeys[i]);

                ret ^= core::hash<core::u8>()(_this.m_subPassCount);

                return ret;
            }
        };
    };
}