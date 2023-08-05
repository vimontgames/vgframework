#pragma once

#include "core/Object/Object.h"

namespace vg::core::io
{
    class Buffer;
}

namespace vg::renderer
{
    struct Batch : public core::Object
    {
        using super = core::Object;

        const char * getClassName() const final { return "Batch"; }

        static bool registerClass(core::IFactory & _factory);
        static bool registerProperties(core::IClassDesc & _desc);

        Batch(const core::string & _name = "", core::IObject * _parent = nullptr);
        Batch(const Batch & _other);
        ~Batch();

        bool read(core::io::Buffer & _buffer);
        bool write(core::io::Buffer & _buffer) const;

        core::u32 count = 0;    // index or vertex count
        core::u32 offset = 0;   // start index or start vertex
    };   
}