#include "engine/Precomp.h"
#include "EngineOptions.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(EngineOptions, "Engine Options");

    //--------------------------------------------------------------------------------------
    bool EngineOptions::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEx(EngineOptions, m_projectPath, "Project", IProperty::Flags::IsFolder);

        registerPropertyEx(EngineOptions, m_startWorld, "World", IProperty::Flags::IsFile);
        setPropertyDefaultFolder(EngineOptions, m_startWorld, "data/Worlds")

        // TODO: Move to menu or toolbar instead
        registerPropertyCallback(EngineOptions, load, "Load");
        registerPropertyCallbackEx(EngineOptions, save, "Save", IProperty::Flags::SameLine);

        return true;
    }

    static const char * filename = "Engine.xml";

    //--------------------------------------------------------------------------------------
    EngineOptions::EngineOptions(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {
        load(this);
    }

    //--------------------------------------------------------------------------------------
    bool EngineOptions::load(IObject * _object)
    {
        const auto * factory = Kernel::getFactory();
        if (factory->loadFromXML(_object, filename))
        {
            auto * engineOptions = static_cast<EngineOptions *>(_object);
            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool EngineOptions::save(IObject * _object)
    {
        const auto * factory = Kernel::getFactory();
        return factory->saveToXML(_object, filename);
    }

    //--------------------------------------------------------------------------------------
    void EngineOptions::OnPropertyChanged(IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {

    }

    //--------------------------------------------------------------------------------------
    const core::string & EngineOptions::GetProjectPath() const
    {
        return m_projectPath;
    }

    //--------------------------------------------------------------------------------------
    const core::string & EngineOptions::GetStartWorld() const
    {
        return m_startWorld;
    }
}