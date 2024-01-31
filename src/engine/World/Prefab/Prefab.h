#pragma once

#include "engine/World/GameObjectHierarchy.h"

namespace vg::engine
{
    class Prefab : public GameObjectHierarchy
    {
    public:
        VG_CLASS_DECL(Prefab, GameObjectHierarchy);

        Prefab(const core::string & _name, core::IObject * _parent);
        ~Prefab();
    };
}