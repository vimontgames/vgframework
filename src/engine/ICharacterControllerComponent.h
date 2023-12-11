#pragma once

#include "core/Component/Component.h"

namespace vg::engine
{
    class ICharacterControllerComponent : public core::Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ICharacterControllerComponent, core::Component);

        virtual core::float3 GetVelocity() const = 0;
        virtual void SetVelocity(const core::float3 & _velocity) = 0;
    };
}