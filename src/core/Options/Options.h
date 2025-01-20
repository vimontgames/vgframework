#pragma once
#include "core/IOptions.h"

namespace vg::core
{
    class Options : public IOptions
    {
    public:
        VG_CLASS_DECL(Options, IOptions);

        Options(const string & _name, IObject * _parent = nullptr);

        void    Update  () override;
        bool    Load    (bool _async) override;
        bool    Save    () override;

    private:
        bool    m_reload = false;
    };
}