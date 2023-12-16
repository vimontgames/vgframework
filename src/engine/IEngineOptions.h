#pragma once
#include "core/Options/Options.h"

namespace vg::engine
{
    class IEngineOptions : public core::Options
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IEngineOptions, core::Options);

        virtual const core::string & GetProjectPath() const = 0;
        virtual const core::string & GetStartWorld() const = 0;
    };
}