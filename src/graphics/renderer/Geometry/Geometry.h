#pragma once

#include "core/Object/Object.h"

namespace vg::graphics::renderer
{
    class Geometry : public core::Object
    {
    public:
        Geometry(const core::string & _name) : 
            core::Object(_name)
        {

        }
    };
}