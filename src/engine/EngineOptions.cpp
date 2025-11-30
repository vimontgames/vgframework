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

        registerPropertyGroupBegin(EngineOptions, "Animation");
        {
            registerPropertyEnumBitfield(EngineOptions, AnimationOptionFlags, m_animationOptionFlags, "Flags");
            setPropertyDescription(EngineOptions, m_animationOptionFlags, "Option flags for animations");
        }
        registerPropertyGroupEnd(EngineOptions);

        registerPropertyGroupBegin(EngineOptions, "Loading");
        {
            registerPropertyEnumBitfield(EngineOptions, LoadingOptionFlags, m_loadingOptionFlags, "Flags");
            setPropertyDescription(EngineOptions, m_loadingOptionFlags, "Option flags for loading");
        }
        registerPropertyGroupEnd(EngineOptions);

        registerPropertyGroupBegin(EngineOptions, "Memory");
        {
            registerPropertyGroupBegin(EngineOptions, "CPU");
            {
                registerPropertyGroupBegin(EngineOptions, "Physical");
                {
                    registerPropertyEx(EngineOptions, m_cpuMemory.m_totalPhys, "Total", PropertyFlags::ReadOnly | PropertyFlags::Transient);
                    setPropertyDescription(EngineOptions, m_cpuMemory.m_totalPhys, "Total physical memory installed (MB)");

                    registerPropertyEx(EngineOptions, m_cpuMemory.m_usedPhys, "Used", PropertyFlags::ReadOnly | PropertyFlags::Transient);
                    setPropertyDescription(EngineOptions, m_cpuMemory.m_usedPhys, "Used physical memory (MB)");

                    registerPropertyEx(EngineOptions, m_cpuMemory.m_availPhys, "Available", PropertyFlags::ReadOnly | PropertyFlags::Transient);
                    setPropertyDescription(EngineOptions, m_cpuMemory.m_availPhys, "Available physical memory (MB)");
                }
                registerPropertyGroupEnd(EngineOptions);

                registerPropertyGroupBegin(EngineOptions, "Virtual");
                {
                    registerPropertyEx(EngineOptions, m_cpuMemory.m_totalPageFile, "Total", PropertyFlags::ReadOnly | PropertyFlags::Transient);
                    setPropertyDescription(EngineOptions, m_cpuMemory.m_totalPageFile, "Total virtual memory (MB)");

                    registerPropertyEx(EngineOptions, m_cpuMemory.m_usedPageFile, "Used", PropertyFlags::ReadOnly | PropertyFlags::Transient);
                    setPropertyDescription(EngineOptions, m_cpuMemory.m_usedPageFile, "Used virtual memory (MB)");

                    registerPropertyEx(EngineOptions, m_cpuMemory.m_availPageFile, "Available", PropertyFlags::ReadOnly | PropertyFlags::Transient);
                    setPropertyDescription(EngineOptions, m_cpuMemory.m_availPageFile, "Available virtual memory (MB)");
                }
                registerPropertyGroupEnd(EngineOptions);
            }
            registerPropertyGroupEnd(EngineOptions);

            registerPropertyGroupBegin(EngineOptions, "GPU");
            {
                registerPropertyGroupBegin(EngineOptions, "Local");
                {
                    registerPropertyEx(EngineOptions, m_gpuMemory.m_budgetLocal, "Budget", PropertyFlags::ReadOnly | PropertyFlags::Transient);
                    setPropertyDescription(EngineOptions, m_gpuMemory.m_budgetLocal, "Local video memory available to the application (MB)");

                    registerPropertyEx(EngineOptions, m_gpuMemory.m_currentUsageLocal, "Used", PropertyFlags::ReadOnly | PropertyFlags::Transient);
                    setPropertyDescription(EngineOptions, m_gpuMemory.m_currentUsageLocal, "Local video memory is currently used (MB)");

                    registerPropertyEx(EngineOptions, m_gpuMemory.m_availableForReservationLocal, "Available", PropertyFlags::ReadOnly | PropertyFlags::Transient);
                    setPropertyDescription(EngineOptions, m_gpuMemory.m_availableForReservationLocal, "Local video memory the application can reserve (MB)");

                    registerPropertyEx(EngineOptions, m_gpuMemory.m_currentReservationLocal, "Reserved", PropertyFlags::ReadOnly | PropertyFlags::Transient);
                    setPropertyDescription(EngineOptions, m_gpuMemory.m_currentReservationLocal, "Local video memory reserved (MB)");
                }
                registerPropertyGroupEnd(EngineOptions);

                registerPropertyGroupBegin(EngineOptions, "Shared");
                {
                    registerPropertyEx(EngineOptions, m_gpuMemory.m_budgetShared, "Budget", PropertyFlags::ReadOnly | PropertyFlags::Transient);
                    setPropertyDescription(EngineOptions, m_gpuMemory.m_budgetShared, "Shared memory available to the application (MB)");

                    registerPropertyEx(EngineOptions, m_gpuMemory.m_currentUsageShared, "Used", PropertyFlags::ReadOnly | PropertyFlags::Transient);
                    setPropertyDescription(EngineOptions, m_gpuMemory.m_currentUsageShared, "Shared memory is currently used (MB)");

                    registerPropertyEx(EngineOptions, m_gpuMemory.m_availableForReservationShared, "Available", PropertyFlags::ReadOnly | PropertyFlags::Transient);
                    setPropertyDescription(EngineOptions, m_gpuMemory.m_availableForReservationShared, "Shared memory the application can reserve (MB)");

                    registerPropertyEx(EngineOptions, m_gpuMemory.m_currentReservationShared, "Reserved", PropertyFlags::ReadOnly | PropertyFlags::Transient);
                    setPropertyDescription(EngineOptions, m_gpuMemory.m_currentReservationShared, "Shared memory reserved (MB)");
                }
                registerPropertyGroupEnd(EngineOptions);
            }
            registerPropertyGroupEnd(EngineOptions);
        }
        registerPropertyGroupEnd(EngineOptions);

        registerPropertyGroupBegin(EngineOptions, "Physics");
        {
            registerOptionalPropertyEnumBitfield(EngineOptions, m_showRigidBodies, physics::ShapeTypeFlags, m_showRigidBodiesMask, "Bodies")
            setPropertyDescription(EngineOptions, m_showRigidBodiesMask, "Show physics bodies of type");

            registerProperty(EngineOptions, m_mergeStaticBodies, "Merge static");
            setPropertyDescription(EngineOptions, m_mergeStaticBodies, "Merge static bodies at init");

            registerPropertyEnumArray(EngineOptions, string, physics::Category, m_physicsCategories, "Category");
            setPropertyDescription(EngineOptions, m_physicsCategories, "Use physics Categories to filter collisions")
        }
        registerPropertyGroupEnd(EngineOptions);

        registerPropertyGroupBegin(EngineOptions, "Project");
        {
            registerPropertyEx(EngineOptions, m_gamePath, "Game Path", PropertyFlags::IsFolder);
            setPropertyDescription(EngineOptions, m_gamePath, "Game root folder used to load game library");

            registerPropertyEx(EngineOptions, m_startWorld, "Start World", PropertyFlags::IsFile);
            setPropertyDescription(EngineOptions, m_startWorld, "Default world to load at startup");
            setPropertyDefaultFolder(EngineOptions, m_startWorld, "data/Worlds")

            registerPropertyEnumArray(EngineOptions, string, core::Tag, m_gameObjectTags, "GameObject Tags");
            setPropertyDescription(EngineOptions, m_gameObjectTags, "Use GameObject Tags to categorize objects (e.g., Player, Enemy ...)")
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

        return true;
    }

    //--------------------------------------------------------------------------------------
    EngineOptions::EngineOptions(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {
        SetFile("Engine.xml");
        Load(false);

        if (auto * gameObjectTagsProp = GetClassDesc()->GetPropertyByName(VG_STRINGIFY(m_gameObjectTags)))
            updateDynamicEnum(*gameObjectTagsProp);

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
        // "Patch" properties using a generic enum type
        IFactory * factory = Kernel::getFactory();

        const auto classDescs = factory->GetClassDescriptors();

        for (auto * desc : classDescs)
        {
            const auto propCount = desc->GetPropertyCount();
            for (uint i = 0; i < propCount; ++i)
            {
                Property * prop = (Property*)desc->GetPropertyByIndex(i);

                if (!strcmp(_prop.GetName(), VG_STRINGIFY(m_gameObjectTags)))
                {
                    if (PropertyType::EnumFlagsU64 == prop->GetType() && !strcmp(VG_STRINGIFY(Tag), prop->GetEnumTypeName()))
                    {
                        for (uint i = 0; i < countof(m_gameObjectTags); ++i)
                            prop->SetEnumName(i, m_gameObjectTags[i]);
                    }
                }
                else if (!strcmp(_prop.GetName(), VG_STRINGIFY(m_physicsCategories)))
                {
                    if ((PropertyType::EnumU8 == prop->GetType() && !strcmp(VG_STRINGIFY(Category), prop->GetEnumTypeName()))
                        || (PropertyType::EnumFlagsU64 == prop->GetType() && !strcmp(VG_STRINGIFY(CategoryFlag), prop->GetEnumTypeName())))
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
    core::Tag EngineOptions::GetGameObjectTag(const core::string & _name) const
    {
        for (uint i = 0; i < countof(m_gameObjectTags); ++i)
        {
            if (_name == m_gameObjectTags[i])
                return (core::Tag)(1ULL<<(core::u64)i);
        }

        VG_WARNING("[Engine] Could not find core::Tag with name \"%s\"", _name.c_str());
        return (core::Tag)0;
    }

    //--------------------------------------------------------------------------------------
    physics::Category EngineOptions::GetPhysicsCategory(const core::string & _name) const
    {
        for (uint i = 0; i < countof(m_physicsCategories); ++i)
        {
            if (_name == m_physicsCategories[i])
                return (physics::Category)i;
        }

        VG_WARNING("[Engine] Could not find physics::Category with name \"%s\"", _name.c_str());
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

    //--------------------------------------------------------------------------------------
    void EngineOptions::SetCpuMemoryInfo(const core::CPUMemoryInfo & _cpuMem)
    {
        m_cpuMemory = _cpuMem;
    }

    //--------------------------------------------------------------------------------------
    const core::CPUMemoryInfo & EngineOptions::GetCpuMemoryInfo() const
    {
        return m_cpuMemory;
    }

    //--------------------------------------------------------------------------------------
    void EngineOptions::SetGpuMemoryInfo(const core::GPUMemoryInfo & _gpuMem)
    {
        m_gpuMemory = _gpuMem;
    }

    //--------------------------------------------------------------------------------------
    const core::GPUMemoryInfo & EngineOptions::GetGpuMemoryInfo() const
    {
        return m_gpuMemory;
    }
}