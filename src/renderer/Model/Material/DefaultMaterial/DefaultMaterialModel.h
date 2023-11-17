#pragma once

#include "renderer/Model/Material/MaterialModel.h"

namespace vg::renderer
{
    class DefaultMaterialModel : public MaterialModel
    {
    public:
        VG_CLASS_DECL(DefaultMaterialModel, MaterialModel);

        DefaultMaterialModel(const core::string & _name, IObject * _parent);
        ~DefaultMaterialModel();
    };
}