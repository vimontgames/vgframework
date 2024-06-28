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

        void        Update              (float _dt) final override;
        void        OnPropertyChanged   (core::IObject * _object, const core::IProperty & _prop, bool _notifyParent);

    protected:
        core::u8    m_u8 = 0;
        core::u8    m_u8_range = 0;

        core::u16   m_u16 = 0;
        core::u16   m_u16_range = 0;

        core::u32   m_u32 = 0;
        core::u32   m_u32_range = 0;
    };
}