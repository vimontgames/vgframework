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

        void            Update              (float _dt) final override;
        void            OnPropertyChanged   (core::IObject * _object, const core::IProperty & _prop, bool _notifyParent);

    protected:
        core::u8        m_u8 = 0;
        core::u8        m_u8_range = 0;

        core::u16       m_u16 = 0;
        core::u16       m_u16_range = 0;

        core::u32       m_u32 = 0;
        core::u32       m_u32_range = 0;

        core::u64       m_u64 = 0;
        core::u64       m_u64_range = 0;

        core::i8        m_i8 = 0;
        core::i8        m_i8_range = 0;
                          
        core::i16       m_i16 = 0;
        core::i16       m_i16_range = 0;
                          
        core::i32       m_i32 = 0;
        core::i32       m_i32_range = 0;

        core::i64       m_i64 = 0;
        core::i64       m_i64_range = 0;

        float           m_float = 0.0f;
        float           m_float_range = 0.0f;
        core::float2    m_float2 = core::float2(1, 2);
        core::float2    m_float2_range = core::float2(3, 4);
        core::float3    m_float3 = core::float3(1, 2, 3);
        core::float4    m_float4 = core::float4(1, 2, 3, 4);
    };
}