#pragma once

#include "graphics/driver/PipelineState/PipelineState_consts.h"

namespace vg::graphics::driver
{
    enum class BlendFactor : core::u32
    {
        Zero = 0,
        One,
        SrcAlpha,
        OneMinusSrcAlpha,
        SrcColor,
        OneMinusSrcColor
    };

    enum class BlendOp : core::u32
    {
        Add = 0,
        Substract,
        RevSubstract,
        Min,
        Max
    };

    struct RenderTargetBlend
    {
        RenderTargetBlend() :
            bits(0x0)
        {
        }

        union 
        {
            struct
            {
                core::u32       enable        : 1;
                core::u32       _pad          : 7;

                BlendFactor     srcBlend      : 4;
                BlendFactor     dstBlend      : 4;
                BlendOp         blendOp       : 4;
                                          
                BlendFactor     srcBlendAlpha : 4;
                BlendFactor     dstBlendAlpha : 4;
                BlendOp         blendOpAlpha  : 4;
            };
            core::u32 bits;
        };
    };

    enum class BlendStateFlags : core::u32
    {
        None                = 0x00000000,
        AlphaToCoverage     = 0x00000001,
        IndependantBlend    = 0x00000002 
    };

    namespace base
    {
        class BlendState
        {
        public:
            BlendState() :
                m_flags(BlendStateFlags::None)
            {
                for (core::uint i = 0; i < maxRenderTarget; ++i)
                    m_bits[i] = 0x0;
            }
            
        protected:
            union
            {
                struct
                {
                    BlendStateFlags     m_flags;
                    RenderTargetBlend   m_renderTargetBlend[maxRenderTarget];
                };                
                core::u32               m_bits[maxRenderTarget+1];
            };
        };
    }
}

#include VG_GFXAPI_HEADER(BlendState)

namespace vg::graphics::driver
{
    class BlendState : public VG_GFXAPI::BlendState
    {
        using super = VG_GFXAPI::BlendState;

    public:

        BlendState(BlendFactor _srcBlend = BlendFactor::One,
                   BlendFactor _dstBlend = BlendFactor::Zero,
                   BlendOp     _blendOp  = BlendOp::Add)
        {
            m_flags = BlendStateFlags::None;


            for (core::uint i = 0; i < maxRenderTarget; ++i)
            {
                auto & rt = m_renderTargetBlend[i];

                rt.srcBlend = _srcBlend;
                rt.dstBlend = _dstBlend;
                rt.blendOp = _blendOp;
                
                rt.srcBlendAlpha = rt.srcBlend;
                rt.dstBlendAlpha = rt.dstBlend;
                rt.blendOpAlpha = rt.blendOp;

                rt.enable = isBlendEnabled(rt);
            }
        }

        BlendState(BlendFactor _srcBlend,
                   BlendFactor _dstBlend,
                   BlendOp     _blendOp ,
                                         
                   BlendFactor _srcBlendAlpha,
                   BlendFactor _dstBlendAlpha,
                   BlendOp     _blendOpAlpha)
        {
            m_flags = BlendStateFlags::None;

            for (core::uint i = 0; i < maxRenderTarget; ++i)
            {
                auto & rt = m_renderTargetBlend[i];

                rt.srcBlend = _srcBlend;
                rt.dstBlend = _dstBlend;
                rt.blendOp = _blendOp;
                
                rt.srcBlendAlpha = _srcBlendAlpha;
                rt.dstBlendAlpha = _dstBlendAlpha;
                rt.blendOpAlpha = _blendOpAlpha;

                rt.enable = isBlendEnabled(rt);
            }
        }

        static bool isBlendEnabled(const RenderTargetBlend & _renderTargetBlend)
        {
            if (_renderTargetBlend.srcBlend != BlendFactor::Zero && _renderTargetBlend.dstBlend != BlendFactor::Zero)
                return true;

            return false;
        }

        inline bool operator == (const BlendState & _other) const
        {
            for (core::uint i = 0; i < maxRenderTarget+1; ++i)
            {
                if (m_bits[i] != _other.m_bits[i])
                    return false;
            }

            return true;
        }

        inline bool operator != (const BlendState & _other) const
        {
            for (core::uint i = 0; i < maxRenderTarget+1; ++i)
            {
                if (m_bits[i] != _other.m_bits[i])
                    return true;
            }

            return false;
        }

        struct hash
        {
            inline core::size_t operator() (const BlendState & _this) const
            {
                VG_STATIC_ASSERT(sizeof(_this) == sizeof(_this.m_bits), "Invalid sizeof in hash function");

                core::size_t hashVal = 0;

                for (core::uint i = 0; i < maxRenderTarget+1; ++i)
                    hashVal ^= std::hash<core::u64>()(_this.m_bits[i]);

                return hashVal; 
            }
        };
    };
}