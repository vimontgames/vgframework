#pragma once

#include "core/Object/Object.h"

namespace vg::engine
{
    class IMaterialList : public core::Object
    {
    public:
        virtual bool AddMaterial() = 0;
        virtual bool RemoveMaterial() = 0;
    };
}