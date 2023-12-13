#pragma once

#include "core/IScene.h"

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    VG_INLINE core::GameObject * Scene::getRoot() const
    {
        return m_root;
    }
}