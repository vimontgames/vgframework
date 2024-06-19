#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"

namespace vg::core
{
    class DynamicPropertyFloat4 : public DynamicProperty
    {
    public:
        VG_CLASS_DECL(DynamicPropertyFloat4, DynamicProperty);

        DynamicPropertyFloat4(const core::string & _name = "", core::IObject * _parent = nullptr);
        DynamicPropertyFloat4(const core::IObject * _object, const core::IProperty * _prop);
        ~DynamicPropertyFloat4();

        bool Set(IObject * _object, IProperty * _prop) final override;

    //private:
        core::float4 m_value;
    };
}