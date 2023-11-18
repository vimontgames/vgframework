#include "MaterialData.h"

using namespace vg::core;

namespace vg::engine
{
    //VG_AUTO_REGISTER_CLASS(MaterialData);

    //--------------------------------------------------------------------------------------
    //bool MaterialData::registerClass(IFactory & _factory)
    //{
    //    if (core::IClassDesc * desc = _factory.registerClassHelper(MaterialData, "Material Data", IClassDesc::Flags::None))
    //        registerProperties(*desc);
    //
    //    return true;
    //}

    //--------------------------------------------------------------------------------------
    bool MaterialData::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyEnumBitfield(MaterialData, renderer::MaterialFlags, m_flags, "Flags");
        
        return true;
    }

    //--------------------------------------------------------------------------------------
    MaterialData::MaterialData(const core::string & _name, IObject * _parent) :
        super(_name, _parent),
        m_flags((renderer::MaterialFlags)0)
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
}