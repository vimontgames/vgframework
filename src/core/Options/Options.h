#pragma once
#include "core/IOptions.h"

namespace vg::core
{
    class Options : public IOptions
    {
    public:
        VG_CLASS_DECL(Options, IOptions);

        Options(const string & _name, IObject * _parent = nullptr);

        virtual bool    Load() override;
        virtual bool    Save() override;
    };
}