#include "DynamicPropertyResource.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(DynamicPropertyResource, "DynamicPropertyResource");

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyResource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalProperty(DynamicPropertyResource, m_enable, m_value, "Value");

        return true;
    }

    //--------------------------------------------------------------------------------------
    DynamicPropertyResource::DynamicPropertyResource(const core::string & _name, core::IObject * _parent) :
        DynamicProperty(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyResource::BackupOriginalValue(const IObject * _object, const IProperty * _prop)
    {
        if (IResource * res = _prop->GetPropertyResource(_object))
        {
            m_original = res->GetResourcePath();
            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyResource::RestoreOriginalValue(IObject * _object, const IProperty * _prop)
    {
        if (IResource * res = _prop->GetPropertyResource(_object))
        {
            res->SetResourcePath(m_original);
            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyResource::ApplyOverride(IObject * _object, const IProperty * _prop)
    {
        if (IResource * res = _prop->GetPropertyResource(_object))
        {
            res->SetResourcePath(m_value);
            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool DynamicPropertyResource::SetOverrideInitValue(const IObject * _object, const IProperty * _prop)
    {
        if (IResource * res = _prop->GetPropertyResource(_object))
        {
            m_value = res->GetResourcePath();
            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    void DynamicPropertyResource::CopyProperty(const IProperty * _prop)
    {
        m_prop = *(Property *)_prop;
        m_prop.SetOffset(VG_OFFSETOF(DynamicPropertyResource, m_value));
    }
}