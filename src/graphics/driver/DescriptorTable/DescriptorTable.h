#pragma once

#include "core/Object/Object.h"

namespace vg::graphics::driver
{
    namespace base
    {
        class DescriptorTable : public core::Object
        {
        public:
            DescriptorTable();
            ~DescriptorTable();
        };
    }
}

#include VG_GFXAPI_HEADER(DescriptorTable)

namespace vg::graphics::driver
{
    class DescriptorTable : public VG_GFXAPI::DescriptorTable
    {
        using super = VG_GFXAPI::DescriptorTable;

    public:
        DescriptorTable();
        ~DescriptorTable();
    };
}