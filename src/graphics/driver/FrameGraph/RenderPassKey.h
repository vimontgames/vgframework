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
        enum Flags
        {
            None    = 0x0,
            Bind    = 0x1,
            Clear   = 0x2,
            Load    = 0x4,
            Store   = 0x8 
        };

        SubPassKey() :
            color(Flags::None),
            depth(Flags::None),
            _pad(0x0000000)
        {

        }

        union
        {
            struct
            {
                union
                {
                    struct
                    {
                        Flags color0 : 4;
                        Flags color1 : 4;
                        Flags color2 : 4;
                        Flags color3 : 4;
                        Flags color4 : 4;
                        Flags color5 : 4;
                        Flags color6 : 4;
                        Flags color7 : 4;
                    };
                    Flags color : 32;
                };
                Flags depth : 4;
                core::u32 _pad : 28;
            };
            core::u32 bits;
        };

        inline void setRenderTargetFlags(core::uint _renderTargetIndex, Flags _flags)
        {
            color = Flags( color | (_flags << (_renderTargetIndex << 2)));
        }

        inline Flags getRenderTargetFlags(core::uint _renderTargetIndex) const
        {
            return Flags((color << (_renderTargetIndex<<2)) & 0xF);
        }

        inline void setDepthStencilFlags(Flags _flags)
        {
            depth = Flags(depth | _flags);
        }

        inline Flags getDepthStencilFlags() const
        {
            return depth;
        }

        inline bool operator == (const SubPassKey & _other) const
        {
            return _other.bits == bits;
        }

        inline bool operator != (const SubPassKey & _other) const
        {
            return !(_other.bits == bits);
        }

        struct hash
        {
            inline core::size_t operator() (const SubPassKey & _this) const
            {
                return core::hash<core::u32>()(_this.bits);
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