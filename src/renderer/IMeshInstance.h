#pragma once

#include "renderer/Instance/GraphicInstance.h"

namespace vg::renderer
{
    class IMeshInstance : public GraphicInstance
    {
    public:
        IMeshInstance(const core::string & _name, core::IObject * _parent) :
            GraphicInstance(_name, _parent)
        {
            
        }

        virtual bool ShowSkeleton() const = 0;
    };
}