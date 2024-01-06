#include "MaterialData.h"

using namespace vg::core;

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    bool MaterialData::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnum(MaterialData, gfx::SurfaceType, m_surfaceType, "Surface Type");
        
        return true;
    }

    //--------------------------------------------------------------------------------------
    MaterialData::MaterialData(const core::string & _name, IObject * _parent) :
        super(_name, _parent),
        m_surfaceType(gfx::SurfaceType::Opaque)
    {

    }

    //--------------------------------------------------------------------------------------
    MaterialData::~MaterialData()
    {

    }

    //--------------------------------------------------------------------------------------
    renderer::IMaterialModel * MaterialData::getMaterialModel() const
    {
        MaterialResourceData * parent = dynamic_cast<MaterialResourceData *>(getParent());
        VG_ASSERT(parent);
        renderer::IMaterialModel * materialModel = parent->m_materialModel;
        VG_ASSERT(materialModel);
        return materialModel;
    }

    //--------------------------------------------------------------------------------------
    void MaterialData::OnPropertyChanged(IObject * _object, const IProperty & _prop, bool _notifyParent)
    {
        auto * material = getMaterialModel();

        if (!strcmp(_prop.getName(), "m_surfaceType"))
            material->SetSurfaceType(m_surfaceType);

        super::OnPropertyChanged(_object, _prop, _notifyParent);
    }
}