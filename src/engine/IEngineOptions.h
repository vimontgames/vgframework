#pragma once
#include "core/Options/Options.h"

namespace vg::physics
{
    enum class Category : core::u8;
}

namespace vg::engine
{
    class IEngineOptions : public core::Options
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IEngineOptions, core::Options);

        virtual const core::string &    GetProjectPath      () const = 0;
        virtual const core::string &    GetStartWorld       () const = 0;

        virtual float                   TryGetTimeScale     (float & _timescale) const = 0;
        virtual bool                    TryGetFixedDT       (float & _fixedDT) const = 0;
        virtual bool                    TryGetMaxDT         (float & _maxDT) const = 0;

        virtual physics::Category       GetPhysicsCategory  (const core::string & _name) const = 0;
    };
}