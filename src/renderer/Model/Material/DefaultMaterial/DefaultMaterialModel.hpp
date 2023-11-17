#include "DefaultMaterialModel.h"

namespace vg::renderer
{
    VG_AUTO_REGISTER_CLASS(DefaultMaterialModel);

    //--------------------------------------------------------------------------------------
    bool DefaultMaterialModel::registerClass(core::IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(DefaultMaterialModel, "Default", IClassDesc::Flags::Model))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool DefaultMaterialModel::registerProperties(core::IClassDesc & _desc)
    {
        return super::registerProperties(_desc);
    }

    //--------------------------------------------------------------------------------------
    DefaultMaterialModel::DefaultMaterialModel(const core::string & _name, IObject * _parent) :
        MaterialModel(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    DefaultMaterialModel::~DefaultMaterialModel()
    {

    }
}