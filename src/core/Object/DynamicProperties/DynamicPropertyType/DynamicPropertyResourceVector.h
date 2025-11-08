#pragma once

#include "core/Object/DynamicProperties/DynamicProperty.h"
#include "core/Resource/Resource.h"

namespace vg::core
{
    class ResourceOverrideData : public core::Object
    {
    public:
        VG_CLASS_DECL(ResourceOverrideData, Object);

        ResourceOverrideData(const string & _name = "", IObject * _parent = nullptr);
        ~ResourceOverrideData();

        bool m_enable = false;
        string m_original;
        string m_value;
    };

    class DynamicPropertyResourceVector : public DynamicProperty
    {
    public:
        VG_CLASS_DECL(DynamicPropertyResourceVector, DynamicProperty);

        DynamicPropertyResourceVector(const string & _name = "", IObject * _parent = nullptr);

        bool BackupOriginalValue(const IObject * _object, const IProperty * _prop) override;
        bool RestoreOriginalValue(IObject * _object, const IProperty * _prop) override;
        bool ApplyOverride(IObject * _object, const IProperty * _prop) override;
        bool SetOverrideInitValue(const IObject * _object, const IProperty * _prop) override;

        void CopyProperty(const IProperty * _prop) override;

        void setValueAt(core::uint _index, const string & _value);
        const string & getValueAt(core::uint _index) const;
        bool isEnabledAt(core::uint _index) const;

    private:
        core::vector<ResourceOverrideData> m_values;
    };
}