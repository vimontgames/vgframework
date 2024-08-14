#pragma once

#include "core/Component/Component.h"

namespace vg::engine
{
    class TestComponent : public core::Component
    {
    public:
        VG_CLASS_DECL(TestComponent, core::Component);

        TestComponent(const core::string & _name, core::IObject * _parent);
        ~TestComponent();

        void            Update              (const Context & _context) final override;

    protected:
        core::u8        m_u8 = 0;
        core::u16       m_u16 = 0;
        core::u32       m_u32 = 0;
        core::u64       m_u64 = 0;

        core::i8        m_i8 = 0;
        core::i16       m_i16 = 0;
        core::i32       m_i32 = 0;
        core::i64       m_i64 = 0;

        float           m_float = 0.0f;

        core::float2    m_float2 = core::float2(1, 2);
        core::float3    m_float3 = core::float3(1, 2, 3);
        core::float4    m_float4 = core::float4(1, 2, 3, 4);

        core::uint2     m_uint2 = core::uint2(1, 2);
        core::uint3     m_uint3 = core::uint3(1, 2, 3);
        core::uint4     m_uint4 = core::uint4(1, 2, 3, 4);

        core::int2      m_int2 = core::int2(1, 2);
        core::int3      m_int3 = core::int3(1, 2, 3);
        core::int4      m_int4 = core::int4(1, 2, 3, 4);
    };
}