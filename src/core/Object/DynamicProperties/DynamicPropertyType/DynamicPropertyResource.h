#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"
#include "core/Resource/Resource.h"

namespace vg::core
{
    class DynamicPropertyResource : public DynamicProperty
    {
    public:
        VG_CLASS_DECL(DynamicPropertyResource, DynamicProperty);

        DynamicPropertyResource(const string & _name = "", IObject * _parent = nullptr);

        bool BackupOriginalValue(const IObject * _object, const IProperty * _prop) override;
        bool RestoreOriginalValue(IObject * _object, const IProperty * _prop) override;
        bool ApplyOverride(IObject * _object, const IProperty * _prop) override;
        bool SetOverrideInitValue(const IObject * _object, const IProperty * _prop) override;

        void CopyProperty(const IProperty * _prop) override;

    //private:
        string m_original;
        string m_value;
    };
}