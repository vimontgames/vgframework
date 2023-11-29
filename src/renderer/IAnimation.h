#pragma once

#include "core/Model/Model.h"

namespace vg::renderer
{
    class IAnimation : public core::Model
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IAnimation, core::Model)

        virtual float GetLength() const = 0;
        virtual float GetFramerate() const = 0;
    };
}