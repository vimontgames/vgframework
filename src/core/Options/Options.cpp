#include "core/Precomp.h"
#include "Options.h"
#include "core/Kernel.h"
#include "core/IFactory.h"
#include "core/IScheduler.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    Options::Options(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    bool Options::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }

    //--------------------------------------------------------------------------------------
    void Options::Update()
    {
        if (m_reload)
        {
            Load(false);
            m_reload = false;
        }
    }

    //--------------------------------------------------------------------------------------
    bool Options::Load(bool _async)
    {
        if (_async)
        {
            m_reload = true;
            return true;
        }
        else
        {
            const auto * factory = Kernel::getFactory();
            if (factory->LoadFromXML(this, GetFile()))
            {
                OnLoad();
                return true;
            }
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Options::Save()
    {
        const auto * factory = Kernel::getFactory();
        return factory->SaveToXML(this, GetFile());
    }
}