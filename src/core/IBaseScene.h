#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IGameObject;
    class IWorld;

    vg_enum(vg::core, BaseSceneType, u8,
        Scene = 0,
        Prefab
    );

    class IBaseScene : public Object
    {
    public:
        virtual void            SetRoot         (IGameObject * _sector) = 0;
        virtual IGameObject *   GetRoot         () const  = 0;

        virtual IWorld *        GetWorld        () const = 0;
        virtual BaseSceneType   GetSceneType    () const = 0;
    };
}