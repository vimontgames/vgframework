#include "core/Precomp.h"
#include "Options.h"
#include "core/Kernel.h"
#include "core/IFactory.h"

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
    bool Options::Load()
    {
        const auto * factory = Kernel::getFactory();
        if (factory->loadFromXML(this, getFile()))
        {
            OnLoad();
            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Options::Save()
    {
        const auto * factory = Kernel::getFactory();
        return factory->saveToXML(this, getFile());
    }
}