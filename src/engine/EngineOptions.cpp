#include "engine/Precomp.h"
#include "EngineOptions.h"
#include "physics/Physics_Consts.h"
#include "core/Object/Property.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(EngineOptions, "Engine Options");

    //--------------------------------------------------------------------------------------
    bool EngineOptions::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyGroupBegin(EngineOptions, "Project");
        {
            registerPropertyEx(EngineOptions, m_gamePath, "Game Path", PropertyFlags::IsFolder);
            setPropertyDescription(EngineOptions, m_gamePath, "Game root folder (used to load game library)");

            registerPropertyEx(EngineOptions, m_startWorld, "Start World", PropertyFlags::IsFile);
            setPropertyDescription(EngineOptions, m_startWorld, "Default world to load at startup");
            setPropertyDefaultFolder(EngineOptions, m_startWorld, "data/Worlds")
        }
        registerPropertyGroupEnd(EngineOptions);

        registerPropertyGroupBegin(EngineOptions, "Physics");
        {
            registerOptionalPropertyEnumBitfield(EngineOptions, m_showRigidBodies, physics::ShapeTypeFlags, m_showRigidBodiesMask, "Bodies")
            setPropertyDescription(EngineOptions, m_showRigidBodiesMask, "Show physics bodies of type");

            registerProperty(EngineOptions, m_mergeStaticBodies, "Merge static");
            setPropertyDescription(EngineOptions, m_mergeStaticBodies, "Merge static bodies at init");

            registerPropertyEnumArray(EngineOptions, string, physics::Category, m_physicsCategories, "Category");
        }
        registerPropertyGroupEnd(EngineOptions);

        registerPropertyGroupBegin(EngineOptions, "Time");
        {
            registerOptionalProperty(EngineOptions, m_useTimeScale, m_timeScale, "Time Scale");
            setPropertyDescription(EngineOptions, m_timeScale, "Adjusts time speed in the engine, affecting all time-dependent systems");
            setPropertyRange(EngineOptions, m_timeScale, float2(0.0f, 16.0f));

            registerOptionalProperty(EngineOptions, m_useMaxDT, m_maxDT, "Max DT");
            setPropertyDescription(EngineOptions, m_maxDT, "Set a maximum time delta between frames (ms) to prevent issues with long frames and ensure consistent physics behavior.");
            setPropertyRange(EngineOptions, m_maxDT, float2(0.0f, 1000.0f / 20.0f));

            registerOptionalProperty(EngineOptions, m_useFixedDT, m_fixedDT, "Fixed DT");
            setPropertyDescription(EngineOptions, m_fixedDT, "Use a fixed time delta between frames (ms)\nThis can be useful when stepping in the debugger");
            setPropertyRange(EngineOptions, m_fixedDT, float2(0.0f, 1000.0f / 20.0f));
        }
        registerPropertyGroupEnd(EngineOptions);

        registerPropertyGroupBegin(EngineOptions, "Animation");
        {
            registerPropertyEnumBitfield(EngineOptions, AnimationOptionFlags, m_animationOptionFlags, "Options");
            setPropertyDescription(EngineOptions, m_animationOptionFlags, "Options for animations");
        }
        registerPropertyGroupEnd(EngineOptions);

        return true;
    }

    //--------------------------------------------------------------------------------------
    EngineOptions::EngineOptions(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {
        setFile("Engine.xml");
        Load();
        if (auto * physicsCategoriesProp = GetClassDesc()->GetPropertyByName(VG_STRINGIFY(m_physicsCategories)))
            updateDynamicEnum(*physicsCategoriesProp);
    }

    //--------------------------------------------------------------------------------------
    void EngineOptions::OnPropertyChanged(IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        updateDynamicEnum(_prop);
    }

    //--------------------------------------------------------------------------------------
    void EngineOptions::updateDynamicEnum(const core::IProperty & _prop)
    {
        if (!strcmp(_prop.GetName(), VG_STRINGIFY(m_physicsCategories)))
        {
            // "Patch" all properties using an enum of type "physics::Category" or "physics::CategoryFlags"
            IFactory * factory = Kernel::getFactory();

            const auto classDescs = factory->GetClassDescriptors();

            for (auto * desc : classDescs)
            {
                const auto propCount = desc->GetPropertyCount();
                for (uint i = 0; i < propCount; ++i)
                {
                    Property * prop = (Property*)desc->GetPropertyByIndex(i);

                    if ( (PropertyType::EnumU8  == prop->GetType() && !strcmp("Category", prop->GetEnumTypeName())) 
                      || (PropertyType::EnumFlagsU64 == prop->GetType() && !strcmp("CategoryFlag", prop->GetEnumTypeName())))
                    {
                        for (uint i = 0; i < countof(m_physicsCategories); ++i)
                            prop->SetEnumName(i, m_physicsCategories[i]);
                    }
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    core::vector<core::string> EngineOptions::getPhysicsCategoryNames() const
    {
        vector<string> names(enumCount<physics::Category>());
        for (uint i = 0; i < names.size(); ++i)
            names[i] = m_physicsCategories[i];
        return names;
    }

    //--------------------------------------------------------------------------------------
    physics::Category EngineOptions::GetPhysicsCategory(const core::string & _name) const
    {
        for (uint i = 0; i < countof(m_physicsCategories); ++i)
        {
            if (_name == m_physicsCategories[i])
                return (physics::Category)i;
        }

        VG_WARNING("[Physics] Could not find physics::Category with name \"%s\"", _name.c_str());
        return (physics::Category)0;
    }

    //--------------------------------------------------------------------------------------
    const core::string & EngineOptions::GetProjectPath() const
    {
        return m_gamePath;
    }

    //--------------------------------------------------------------------------------------
    const core::string & EngineOptions::GetStartWorld() const
    {
        return m_startWorld;
    }

    //--------------------------------------------------------------------------------------
    float EngineOptions::TryGetTimeScale(float & _timescale) const
    { 
        if (m_useTimeScale)
        {
            _timescale = m_timeScale;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool EngineOptions::TryGetFixedDT(float & _fixedDT) const 
    {
        if (m_useFixedDT) 
        { 
            _fixedDT = m_fixedDT; 
            return true; 
        } 

        return false;
    }
    //--------------------------------------------------------------------------------------
    bool EngineOptions::TryGetMaxDT(float & _maxDT) const 
    { 
        if (m_useMaxDT) 
        { 
            _maxDT = m_maxDT; 
            return true; 
        } 
        
        return false; 
    }
}