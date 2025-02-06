#pragma once

#include "core/IDynamicProperty.h"
#include "core/Object/Property.h"
#include "core/Kernel.h"

namespace vg::core
{
    class DynamicProperty : public IDynamicProperty
    {
    public:
        VG_CLASS_DECL(DynamicProperty, IDynamicProperty);

        DynamicProperty(const core::string & _name = "", core::IObject * _parent = nullptr);
        ~DynamicProperty();

        void Enable(bool _enable) final override
        {
            m_enable = _enable;
        }

        bool IsEnable() const final override
        {
            return m_enable;
        }

        IProperty * GetProperty() const final override
        {
            return (IProperty*) &m_prop;
        }

        bool RegisterUID() final override;

        virtual void CopyProperty(const IProperty * _prop) = 0;

    protected:
        bool        m_enable = true;
        Property    m_prop = nullptr;
    };

    template <typename T> class DynamicPropertyT : public DynamicProperty
    {
    public:
        DynamicPropertyT(const core::string & _name = "", core::IObject * _parent = nullptr)
        {
            SetName(_name);
        }

        ~DynamicPropertyT()
        {

        }

        void SetValue(T _value)
        {
            m_value = _value;
        }

        virtual T * GetPropertyPtr(const IObject * _object, const IProperty * _prop) const = 0;

        bool ApplyOverride(IObject * _object, const IProperty * _prop) final override
        {
            if (T * value = GetPropertyPtr(_object, _prop))
            {
                _object->SetPropertyValue(*_prop, value, &m_value);
                return true;
            }
            return false;
        }

        bool BackupOriginalValue(const IObject * _object, const IProperty * _prop) final override
        {
            if (T * value = GetPropertyPtr(_object, _prop))
            {
                m_original = *value;
                return true;
            }
            return false;
        }

        bool RestoreOriginalValue(IObject * _object, const IProperty * _prop) final override
        {
            if (T * value = GetPropertyPtr(_object, _prop))
            {
                _object->SetPropertyValue(*_prop, value, &m_original);
                //*value = m_original;
                return true;
            }
            return false;
        }

        bool SetOverrideInitValue(const IObject * _object, const IProperty * _prop) final override
        {
            if (T * value = GetPropertyPtr(_object, _prop))
            {
                m_value = *value;
                return true;
            }
            return false;
        }

        void CopyProperty(const IProperty * _prop) override
        {
            m_prop = *(Property *)_prop;
            m_prop.SetOffset(VG_OFFSETOF(DynamicPropertyT, m_value));
        }

    protected:
        T m_value;
        T m_original;
    };
}