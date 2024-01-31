#pragma once

#include "engine/World/GameObjectHierarchy.h"

namespace vg::engine
{
    class Scene : public GameObjectHierarchy
    {
    public:
        VG_CLASS_DECL(Scene, GameObjectHierarchy);

        Scene(const core::string & _name, core::IObject * _parent);
        ~Scene();

    };
}