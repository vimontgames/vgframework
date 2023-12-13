#include "WorldResourceData.h"

#define MAX_SCENE_PER_WORLD 256

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(WorldResourceData, "World Resource Data");

    //--------------------------------------------------------------------------------------
    bool WorldResourceData::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyObjectPtr(WorldResourceData, m_world, "World");
        registerPropertyResourcePtrVector(WorldResourceData, m_sceneResources, "Scenes");

        return true;
    }

    //--------------------------------------------------------------------------------------
    WorldResourceData::WorldResourceData(const core::string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        
    }

    //--------------------------------------------------------------------------------------
    WorldResourceData::~WorldResourceData()
    {
        // The copy here is intentional, because release scene resources will remove them from the container
        auto scenesToRelease = m_sceneResources;
        for (uint i = 0; i < scenesToRelease.size(); ++i)
            VG_SAFE_RELEASE(scenesToRelease[i]);
        m_sceneResources.clear();

        VG_SAFE_RELEASE(m_world);
    }

    //--------------------------------------------------------------------------------------
    void WorldResourceData::createWorld()
    {
        VG_SAFE_RELEASE(m_world);
        IFactory * factory = Kernel::getFactory();
        m_world = (IWorld *)factory->createObject("World");
        m_world->setParent((IObject *)this);
    }

    //--------------------------------------------------------------------------------------
    void WorldResourceData::OnPropertyChanged(IObject * _object, const IProperty & _prop, bool _notifyParent)
    {
        // [...]

        super::OnPropertyChanged(_object, _prop, _notifyParent);
    }
}