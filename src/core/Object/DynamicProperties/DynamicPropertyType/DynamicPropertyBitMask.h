#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"
#include "core/Misc/BitMask/BitMask.h"

namespace vg::core
{
    class DynamicPropertyBitMask : public DynamicPropertyT<BitMask>
    {
    public:
        VG_CLASS_DECL(DynamicPropertyBitMask, DynamicProperty);

        DynamicPropertyBitMask(const core::string & _name = "", core::IObject * _parent = nullptr);

        BitMask * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const final override;
    };
}