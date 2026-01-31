#include "DynamicPropertyResource.h"

namespace vg::core
{
    VG_REGISTER_CLASS(ResourceOverrideData, "Resource override data");

    //--------------------------------------------------------------------------------------
    ResourceOverrideData::ResourceOverrideData(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    ResourceOverrideData::~ResourceOverrideData()
    {

    }

    //--------------------------------------------------------------------------------------
    void * ResizeResourceOverrideDataVector(IObject * _parent, uint _offset, uint _count)
    {
        auto vec = (core::vector<ResourceOverrideData> *)(uint_ptr(_parent) + _offset);
        vec->clear();
        vec->resize(_count);
        return vec->data();
    }

    //--------------------------------------------------------------------------------------
    bool ResourceOverrideData::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalProperty(ResourceOverrideData, m_enable, m_value, "Value");
        registerResizeVectorFunc(ResourceOverrideData, ResizeResourceOverrideDataVector);

        return true;
    }

    VG_REGISTER_CLASS(DynamicPropertyResourceVector, "DynamicPropertyResourceVectorItem");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyResourceVector::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(DynamicPropertyResourceVector, m_enable, "Enable");
        registerPropertyObjectVector(DynamicPropertyResourceVector, m_values, ResourceOverrideData, "Values");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyResourceVector::DynamicPropertyResourceVector(const core::string & _name, core::IObject * _parent) :
        DynamicProperty(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyResourceVector::BackupOriginalValue(const IObject * _object, const IProperty * _prop)
    {
        const auto count = _prop->GetPropertyResourceVectorCount(_object);
        m_values.resize(count);

        for (uint i = 0; i < count; ++i)
        {
            if (IResource * res = _prop->GetPropertyResourceVectorElement(_object, i))
            {
                m_values[i].m_original = res->GetResourcePath();
            }
        }
        return true;
    }

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyResourceVector::RestoreOriginalValue(IObject * _object, const IProperty * _prop)
    {
        const auto count = _prop->GetPropertyResourceVectorCount(_object);
        m_values.resize(count);

        for (uint i = 0; i < count; ++i)
        {
            if (IResource * res = _prop->GetPropertyResourceVectorElement(_object, i))
            {
                res->SetResourcePath(m_values[i].m_original);
            }
        }
        return true;
    }

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyResourceVector::ApplyOverride(IObject * _object, const IProperty * _prop)
    {
        const auto count = _prop->GetPropertyResourceVectorCount(_object);
        m_values.resize(count);

        for (uint i = 0; i < count; ++i)
        {
            if (isEnabledAt(i))
            {
                if (IResource * res = _prop->GetPropertyResourceVectorElement(_object, i))
                {
                    res->SetResourcePath(getValueAt(i));
                }
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyResourceVector::SetOverrideInitValue(const IObject * _object, const IProperty * _prop)
    {
        const auto count = _prop->GetPropertyResourceVectorCount(_object);
        m_values.resize(count);

        for (uint i = 0; i < count; ++i)
        {
            if (IResource * res = _prop->GetPropertyResourceVectorElement(_object, i))
            {
                m_values[i].m_value = res->GetResourcePath();
            }
        }
        return true;
    }

    //--------------------------------------------------------------------------------------
    void DynamicPropertyResourceVector::CopyProperty(const IProperty * _prop)
    {
        m_prop = *(Property *)_prop;
        m_prop.SetOffset(VG_OFFSETOF(DynamicPropertyResourceVector, m_values));
    }

    //--------------------------------------------------------------------------------------
    void DynamicPropertyResourceVector::setValueAt(core::uint _index, const string & _value)
    {
        VG_ASSERT(_index < m_values.size());
        if (_index < m_values.size())
        {
            m_values[_index].m_value = _value;
            m_values[_index].m_enable = true;
        }
    }
    
    //--------------------------------------------------------------------------------------
    const string & DynamicPropertyResourceVector::getValueAt(core::uint _index) const
    {
        VG_ASSERT(_index < m_values.size());
        return m_values[_index].m_value;
    }

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyResourceVector::isEnabledAt(core::uint _index) const
    {
        if (_index < m_values.size())
            return m_values[_index].m_enable;
        else
            return false;
    }
}