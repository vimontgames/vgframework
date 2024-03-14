#pragma once
#include "core/Component/Component.h"

namespace vg::engine
{
    class ISnapComponent : public core::Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ISnapComponent, core::Component);

        virtual bool TryGetSnapTranslate    (core::float3 & _translate) const = 0;
        virtual bool TryGetSnapRotate       (float & _rotate) const = 0;
        virtual bool TryGetSnapScale        (float & _scale) const = 0;
    };
}