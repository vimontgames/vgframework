#pragma once

#include "core/Model/Model.h"

namespace vg::renderer
{
    class IMaterialModel;

    class IGraphicModel : public core::Model
    {
    public:
        IGraphicModel(const core::string & _name, core::IObject * _parent) :
            core::Model(_name, _parent)
        {

        }

        virtual core::uint GetMaterialCount() const = 0;
        virtual IMaterialModel * GetMaterial(core::uint _index) const = 0;
    };
}