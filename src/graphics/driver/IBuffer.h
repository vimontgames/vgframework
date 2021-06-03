#pragma once

#include "core/Model/Model.h"

namespace vg::graphics::driver
{
    class IBuffer : public core::Model
    {
        using super = core::Model;

    public:
        IBuffer(const core::string & _name, core::IObject * _parent) :
            super(_name, _parent)
        {

        }
    };
}