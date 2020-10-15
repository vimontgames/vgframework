#pragma once

#include "FrameGraph_consts.h"
#include "graphics/driver/Resource/Texture_consts.h"
#include "graphics/driver/PipelineState/PipelineState_consts.h"

namespace vg::graphics::driver
{
    //--------------------------------------------------------------------------------------
    // Descriptor for a subPass used in RenderPass
    // bits [0..31] used for up to maxRenderTarget (8) render targets
    // bits [32..35] used for depthStencil
    // bits [36..63] padding
    //--------------------------------------------------------------------------------------
    struct SubPassKey
    {
        enum class AttachmentFlags : core::u8
        {
            Discard         = 0x00,
            Clear           = 0x01,  // clear before use
            Preserve        = 0x02,  // init with previous contents of attachment

            RenderTarget    = 0x04,  // attachment will be used as a render target during this pass
            Present         = 0x08,  // attachment will be used for 'Present' right after this pass
        };

        struct AttachmentInfo
        {
            AttachmentInfo(AttachmentFlags _flags = AttachmentFlags::Discard, ResourceState _begin = ResourceState::Undefined, ResourceState _end = ResourceState::Undefined) :
                flags(_flags),
                begin(_begin),
                end(_end)
            {

            }

            union
            {
                struct
                {

                    AttachmentFlags flags   : 4;
                    ResourceState   begin   : 2;
                    ResourceState   end     : 2;
                };
                core::u8 bits;
            };
        };

        AttachmentInfo color[maxRenderTarget];
        AttachmentInfo depth;

        inline void setColorAttachmentInfo(core::uint _index, const AttachmentInfo & _attachmentInfos)
        {
            VG_ASSERT(_index < maxRenderTarget);
            color[_index] = _attachmentInfos;
        }

        inline const AttachmentInfo & getColorAttachmentInfo(core::uint _index) const
        {
            VG_ASSERT(_index < maxRenderTarget);
            return color[_index];
        }

        inline void setDepthStencilAttachmentInfo(const AttachmentInfo & _attachmentInfos)
        {
            depth = _attachmentInfos;
        }

        inline const AttachmentInfo & getDepthStencilAttachmentInfo() const
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