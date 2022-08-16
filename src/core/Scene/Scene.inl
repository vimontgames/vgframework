#pragma once

#include "core/IScene.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    VG_INLINE GameObject * Scene::getRoot() const
    {
        return m_root;
    }
}