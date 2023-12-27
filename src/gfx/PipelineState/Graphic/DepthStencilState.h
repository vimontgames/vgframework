#pragma once

#include "gfx/PipelineState/Graphic/GraphicPipelineState_consts.h"

namespace vg::gfx
{
    enum class ComparisonFunc : core::u16
    {
        Always = 0,
        Never,
        Equal,
        NotEqual,
        Less,
        LessEqual,
        Greater,
        GreaterEqual
    };

    enum class StencilOp : core::u16
    {
        Zero = 0,
        Keep,
        Replace,
        IncrementSat,
        DecrementSat,
        Invert,
        IncrementWrap,
        DecrementWrap
    };

    struct StencilState
    {
        StencilState(ComparisonFunc _func = ComparisonFunc::Always, StencilOp _passOp = StencilOp::Keep, StencilOp _failOp = StencilOp::Keep, StencilOp _depthFailOp = StencilOp::Keep) :
            func(_func),
            passOp(_passOp),
            failOp(_failOp),
            depthFailOp(_depthFailOp)
        {

        }

        union
        {
            struct  
            {
                ComparisonFunc  func        : 4;    // 3
                StencilOp       passOp      : 4;    // 3
                StencilOp       failOp      : 4;    // 3
                StencilOp       depthFailOp : 4;    // 3
            };
            core::u16 m_bits;
        };
    };

    namespace base
    {
        class DepthStencilState
        {
        public:
            DepthStencilState() :
                m_bits(0)
            {
                
            }

        protected:
            union
            {
                struct
                {
                    core::u16       m_depthEnable       : 1;
                    core::u16       m_depthWrite        : 1;
                    ComparisonFunc  m_depthFunc         : 13; 
                    core::u16       m_stencilEnable     : 1;
                    core::u16       m_stencilReadMask   : 8;    
                    core::u16       m_stencilWriteMask  : 8;    
                    StencilState    m_stencilFront;
                    StencilState    m_stencilBack;

                };
                core::u64       m_bits;
            };
        };
    }
}

#include VG_GFXAPI_HEADER(DepthStencilState)

namespace vg::gfx
{
    class DepthStencilState : public VG_GFXAPI::DepthStencilState
    {
        using super = VG_GFXAPI::DepthStencilState;

    public:

        DepthStencilState(bool _depthEnable = false, bool _depthWrite = false, ComparisonFunc _depthFunc = ComparisonFunc::Always, 
                          bool _stencilEnable = false, core::u8 _stencilReadMask = 0xFF, core::u8 _stencilWriteMask = 0xFF, 
                          StencilState _stencilFront = { ComparisonFunc::Always, StencilOp::Keep, StencilOp::Keep, StencilOp::Keep }, 
                          StencilState _stencilBack  = { ComparisonFunc::Always, StencilOp::Keep, StencilOp::Keep, StencilOp::Keep } )
        {
            if (_depthEnable)
            {
                m_depthEnable = true;
                m_depthWrite = _depthWrite;
                m_depthFunc = _depthFunc;
            }
            else
            {
                m_depthEnable = false;
                m_depthWrite = false;
                m_depthFunc = ComparisonFunc::Always;
            }

            if (_stencilEnable)
            {
                m_stencilEnable = true;
                m_stencilReadMask = _stencilReadMask;
                m_stencilWriteMask = _stencilWriteMask;
                m_stencilFront = _stencilFront;
                m_stencilBack = _stencilBack;
            }
            else
            {
                m_stencilEnable = false;
                m_stencilReadMask = 0x00;
                m_stencilWriteMask = 0x00;
                m_stencilFront = { ComparisonFunc::Always, StencilOp::Keep, StencilOp::Keep, StencilOp::Keep };
                m_stencilBack =  { ComparisonFunc::Always, StencilOp::Keep, StencilOp::Keep, StencilOp::Keep };
            }            
        }

        inline bool operator == (const DepthStencilState & _other) const
        {
            return _other.m_bits == m_bits;
        }

        inline bool operator != (const DepthStencilState & _other) const
        {
            return !(_other == *this);
        }

        struct hash
        {
            inline core::size_t operator() (const DepthStencilState & _this) const
            {
                VG_STATIC_ASSERT(sizeof(_this) == sizeof(_this.m_bits), "Invalid sizeof in hash function");
                core::size_t hashVal = std::hash<core::u64>()(_this.m_bits);
                return hashVal;
            }
        };
    };
}