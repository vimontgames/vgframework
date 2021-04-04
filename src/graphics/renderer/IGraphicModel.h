#pragma once

#include "core/Model/Model.h"

namespace vg::graphics::renderer
{
    class IGraphicModel : public core::Model
    {
    public:
        IGraphicModel(const core::string & _name, core::IObject * _parent) :
            core::Model(_name, _parent)
        {

        }
    };
}