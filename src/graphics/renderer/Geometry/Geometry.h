#pragma once

#include "core/Object/Object.h"

namespace vg::graphics::renderer
{
    class Geometry : public core::ObjectPointer
    {
    public:
        Geometry(const core::string & _name, core::IObject * _parent) :
            core::ObjectPointer(_name, _parent)
        {

        }
    };
}