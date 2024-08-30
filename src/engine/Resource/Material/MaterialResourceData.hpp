#include "MaterialResourceData.h"
#include "default/DefaultMaterialData.h"    // TODO : abstract factory for material data types
#include "renderer/IMaterialModel.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(MaterialResourceData, "Material Resource Data");

    //--------------------------------------------------------------------------------------
    bool MaterialResourceData::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(MaterialResource, m_name, IProperty::Flags::NotVisible | IProperty::Flags::NotSaved, true);
        registerPropertyObject(MaterialResourceData, m_materialModelType, "Material Model");
        registerPropertyObjectPtrEx(MaterialResourceData, m_data, "Material Data", IProperty::Flags::Flatten);

        return true;
    }

    //--------------------------------------------------------------------------------------
    MaterialResourceData::MaterialResourceData(const core::string & _name, IObject * _parent) :
        Object(_name, _parent)
    {
        
    }

    //--------------------------------------------------------------------------------------
    MaterialResourceData::~MaterialResourceData()
    {
        VG_SAFE_RELEASE(m_data);
        VG_SAFE_RELEASE(m_materialModel);
    }

    //--------------------------------------------------------------------------------------
    void MaterialResourceData::CreateRendererMaterial()
    {
        if (m_data)
        {
            VG_SAFE_RELEASE(m_materialModel);
            m_materialModel = m_data->CreateRendererMaterialModel();
        }
    }

    //--------------------------------------------------------------------------------------
    void MaterialResourceData::OnPropertyChanged(IObject * _object, const IProperty & _prop, bool _notifyParent)
    {
        // When shader template changes, alloc the new structure
        if (!strcmp(_prop.GetName(), "m_shader"))
        {
            VG_SAFE_RELEASE(m_data);

            const string * shader = _prop.GetPropertyString(_object);
            if (nullptr != shader)
            {
                // TODO : abstract factory using 'shader' string
                m_data = new DefaultMaterialData("Default", this); 
                CreateRendererMaterial();
            }
        }

        super::OnPropertyChanged(_object, _prop, _notifyParent);
    }
}