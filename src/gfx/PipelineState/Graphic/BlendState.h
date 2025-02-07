#pragma once

#include "gfx/PipelineState/Graphic/GraphicPipelineState_consts.h"

namespace vg::gfx
{
    vg_enum_class(vg::gfx, BlendFactor, core::u32,
        Zero = 0,
        One,
        SrcAlpha,
        OneMinusSrcAlpha,
        SrcColor,
        OneMinusSrcColor,
        DstAlpha,
        OneMinusDstAlpha,
        DstColor,
        OneMinusDstColor
    );

    vg_enum_class(vg::gfx, BlendOp, core::u32,
        Add = 0,
        Substract,
        RevSubstract,
        Min,
        Max
    );

    vg_enum(vg::gfx, ColorWrite, core::u32,
        Red     = 0x01,
        Green   = 0x02,
        Blue    = 0x04,
        Alpha   = 0x08
    );

    struct RenderTargetBlend
    {
        RenderTargetBlend() :
            bits(0x0)
        {
            colorWrite = ColorWrite(ColorWrite::Red | ColorWrite::Green | ColorWrite::Blue | ColorWrite::Alpha);
        }

        union 
        {
            struct
            {
                core::u32       enable          : 1;
                ColorWrite      colorWrite      : 4;
                core::u32       _pad            : 3;

                BlendFactor     srcBlend        : 4;
                BlendFactor     dstBlend        : 4;
                BlendOp         blendOp         : 4;
                                          
                BlendFactor     srcBlendAlpha   : 4;
                BlendFactor     dstBlendAlpha   : 4;
                BlendOp         blendOpAlpha    : 4;
            };
            core::u32 bits;
        };
    };

    VG_STATIC_ASSERT(sizeof(RenderTargetBlend) == sizeof(core::u32), "Invalid sizeof(RenderTargetBlend)");

    enum class BlendStateFlags : core::u32
    {
        AlphaToCoverage     = 0x00000001,
        IndependantBlend    = 0x00000002 
    };

    namespace base
    {
        class BlendState
        {
        public:
            BlendState() :
                m_flags((BlendStateFlags)0x0)
            {
                for (core::uint i = 0; i < core::countof(m_bits); ++i)
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

#ifdef VG_DX12
#include "dx12/BlendState_dx12.h"
#elif defined(VG_VULKAN)
#include "vulkan/BlendState_vulkan.h"
#else
#error Undefined GFXAPI
#endif

namespace vg::gfx
{
    class BlendState : public VG_GFXAPI::BlendState
    {
        using super = VG_GFXAPI::BlendState;

    public:

        BlendState(BlendFactor _srcBlend = BlendFactor::One,
                   BlendFactor _dstBlend = BlendFactor::Zero,
                   BlendOp     _blendOp  = BlendOp::Add, 
                   ColorWrite  _colorWrite = ColorWrite(ColorWrite::Red | ColorWrite::Green | ColorWrite::Blue | ColorWrite::Alpha))
        {
            m_flags = (BlendStateFlags)0x0;

            for (core::uint i = 0; i < maxRenderTarget; ++i)
            {
                auto & rt = m_renderTargetBlend[i];

                rt.colorWrite = _colorWrite;
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
                   BlendOp     _blendOpAlpha,
                   
                   ColorWrite  _colorWrite = ColorWrite(ColorWrite::Red | ColorWrite::Green | ColorWrite::Blue | ColorWrite::Alpha))
        {
            m_flags = (BlendStateFlags)0x0;

            for (core::uint i = 0; i < maxRenderTarget; ++i)
            {
                auto & rt = m_renderTargetBlend[i];

                rt.colorWrite = _colorWrite;
                rt.srcBlend = _srcBlend;
                rt.dstBlend = _dstBlend;
                rt.blendOp = _blendOp;
                
                rt.srcBlendAlpha = _srcBlendAlpha;
                rt.dstBlendAlpha = _dstBlendAlpha;
                rt.blendOpAlpha = _blendOpAlpha;

                rt.enable = isBlendEnabled(rt);
            }
        }

        inline void setColorWriteMask(core::uint _renderTargetIndex, ColorWrite _colorWriteMask)
        {
            VG_ASSERT(_renderTargetIndex < maxRenderTarget);
            m_renderTargetBlend[_renderTargetIndex].colorWrite = _colorWriteMask;
        }

        inline void enableAlphaToCoverage(bool _enable)
        {
            if (_enable)
                m_flags = (BlendStateFlags)(core::u32(m_flags) | core::u32(BlendStateFlags::AlphaToCoverage));
            else
                m_flags = (BlendStateFlags)(core::u32(m_flags) & ~core::u32(BlendStateFlags::AlphaToCoverage));
        }

        inline void enableIndependantBlend(bool _enable)
        {
            if (_enable)
                m_flags = (BlendStateFlags)(core::u32(m_flags) | core::u32(BlendStateFlags::IndependantBlend));
            else
                m_flags = (BlendStateFlags)(core::u32(m_flags) & ~core::u32(BlendStateFlags::IndependantBlend));
        }

        static bool isBlendEnabled(const RenderTargetBlend & _renderTargetBlend)
        {
            if (_renderTargetBlend.srcBlend == BlendFactor::One && _renderTargetBlend.dstBlend == BlendFactor::Zero)
                return false;

            return true;
        }

        inline bool operator == (const BlendState & _other) const
        {
            for (core::uint i = 0; i < core::countof(m_bits); ++i)
            {
                if (m_bits[i] != _other.m_bits[i])
                    return false;
            }

            return true;
        }

        inline bool operator != (const BlendState & _other) const
        {
            for (core::uint i = 0; i < core::countof(m_bits); ++i)
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

                for (core::uint i = 0; i < core::countof(_this.m_bits); ++i)
                    hashVal ^= std::hash<core::u64>()(_this.m_bits[i]);

                return hashVal; 
            }
        };
    };
}