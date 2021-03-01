#pragma once

#include "graphics/driver/PipelineState/PipelineState_consts.h"

namespace vg::graphics::driver
{
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
                    core::u32   m_depthEnable   : 1;
                    core::u32   m_depthWrite    : 1;
                };
                core::u32       m_bits;
            };
        };
    }
}

#include VG_GFXAPI_HEADER(DepthStencilState)

namespace vg::graphics::driver
{
    class DepthStencilState : public VG_GFXAPI::DepthStencilState
    {
        using super = VG_GFXAPI::DepthStencilState;

    public:

        DepthStencilState(bool _depthEnable = false, bool _depthWrite = false)
        {
            m_depthEnable = _depthEnable;
            m_depthWrite = _depthWrite;
        }

        inline bool operator == (const DepthStencilState & _other) const
        {
            return _other.m_depthEnable == m_depthEnable && _other.m_depthWrite == m_depthWrite;
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
                core::size_t hashVal = std::hash<core::u32>()(_this.m_bits);
                return hashVal;
            }
        };
    };
}