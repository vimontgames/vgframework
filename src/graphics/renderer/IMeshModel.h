#pragma once

#include "IGraphicModel.h"

namespace vg::graphics::renderer
{
    class IMeshModel : public IGraphicModel
    {
    public:
        IMeshModel(const core::string & _name, core::IObject * _parent) :
            IGraphicModel(_name, _parent)
        {

        }
    };
}