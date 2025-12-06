#pragma once

#include "core/Component/Component.h"

namespace vg::engine
{
    class IParticleComponent : public core::Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IParticleComponent, core::Component);

        virtual bool    Play            () = 0;
        virtual bool    Pause           () = 0;
        virtual bool    Stop            () = 0;
        virtual bool    Reset           () = 0;

        virtual bool    UpdateEmitter   (core::uint _index) = 0;
        virtual bool    ResetEmitter    (core::uint _index) = 0;
    };
}