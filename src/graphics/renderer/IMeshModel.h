#pragma once

#include "core/Model/Model.h"

namespace vg::graphics::renderer
{
    class IMeshModel : public core::Model
    {
    public:
        IMeshModel(const core::string & _name) : 
            core::Model(_name)
        {

        }
    };
}