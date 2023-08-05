#pragma once

#include "core/Instance/Instance.h"

namespace vg::renderer
{
    class IGraphicInstance : public core::Instance
    {
    public:
        IGraphicInstance(const core::string & _name, core::IObject * _parent) :
            core::Instance(_name, _parent)
        {

        }
    };
}