#pragma once
#include "core/Object/Object.h"

namespace vg::engine
{
    class IEngineOptions : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IEngineOptions, core::Object);

        virtual const core::string & GetProjectPath() const = 0;
        virtual const core::string & GetStartWorld() const = 0;
    };
}