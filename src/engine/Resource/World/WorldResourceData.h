#pragma once
#include "core/Object/Object.h"

namespace vg::engine
{
    class MaterialData;

    //--------------------------------------------------------------------------------------
    class WorldResourceData : public core::Object
    {
    public:
        VG_CLASS_DECL(WorldResourceData, core::Object);
        WorldResourceData(const core::string & _name = "", IObject * _parent = nullptr);
        ~WorldResourceData();

        void                            createWorld();
        core::IWorld *                  getWorld() const { return m_world; }

        void                            OnPropertyChanged(core::IObject * _object, const core::IProperty & _prop, bool _notifyParent) override;

        core::IWorld *                  m_world = nullptr;
        core::vector<SceneResource*>    m_sceneResources;
    };
}