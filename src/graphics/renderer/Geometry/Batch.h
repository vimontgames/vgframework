#pragma once

#include "core/Object/Object.h"

namespace vg::graphics::renderer
{
    struct Batch : public core::Object
    {
        using super = core::Object;

        const char * getClassName() const final { return "Batch"; }

        static bool registerClass(core::IObjectFactory & _factory);
        static bool registerProperties(core::IObjectDescriptor & _desc);

        Batch(const core::string & _name = "", core::IObject * _parent = nullptr);
        Batch(const Batch & _other);
        ~Batch();

        core::u32 count = 0;    // index or vertex count
        core::u32 offset = 0;   // start index or start vertex
    };   
}