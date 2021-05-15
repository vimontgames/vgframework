#pragma once

#include "core/Model/Model.h"

namespace vg::graphics::driver
{
    class ITexture : public core::Model
    {
        using super = core::Model;

    public:
        ITexture(const core::string & _name, core::IObject * _parent) :
            super(_name, _parent)
        {

        }
    };
}