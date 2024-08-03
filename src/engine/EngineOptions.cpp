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
        setPropertyDescription(EngineOptions, m_projectPath, "Project root folder is used to load game library");

        registerPropertyEx(EngineOptions, m_startWorld, "World", IProperty::Flags::IsFile);
        setPropertyDescription(EngineOptions, m_startWorld, "Default world to load at startup");
        setPropertyDefaultFolder(EngineOptions, m_startWorld, "data/Worlds")

        registerProperty(EngineOptions, m_animationJobs, "Animation Jobs");

        return true;
    }

    //--------------------------------------------------------------------------------------
    EngineOptions::EngineOptions(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {
        setFile("Engine.xml");
        Load();
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