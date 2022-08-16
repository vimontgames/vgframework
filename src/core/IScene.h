#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IGameObject;

    class IScene : public Object
    {
    public:
        virtual void            SetRoot (IGameObject * _sector) = 0;
        virtual IGameObject *   GetRoot () const  = 0;
    };
}