#include "MaterialResourceData.h"
#include "default/DefaultMaterialData.h"    // TODO : abstract factory for material data types
#include "renderer/IMaterialModel.h"

using namespace vg::core;

namespace vg::engine
{
    VG_AUTO_REGISTER_CLASS(MaterialResourceData);

    //--------------------------------------------------------------------------------------
    bool MaterialResourceData::registerClass(IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(MaterialResourceData, "Material Resource Data", IClassDesc::Flags::None))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MaterialResourceData::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyObject(MaterialResourceData, m_materialModelType, "Material Model");
        _desc.registerPropertyObjectPtrEx(MaterialResourceData, m_data, "Material Data", IProperty::Flags::Flatten);

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
    void MaterialResourceData::onPropertyChanged(IObject * _object, const IProperty & _prop)
    {
        // When shader template changes, alloc the new structure
        if (!strcmp(_prop.getName(), "m_shader"))
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

        super::onPropertyChanged(_object, _prop);
    }
}