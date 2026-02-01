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

    class IBaseSceneRenderData : public Object
    {
        public:
            VG_CLASS_DECL_ABSTRACT(IBaseSceneRenderData, Object);

            virtual ~IBaseSceneRenderData() {}
    };

    class IBaseScene : public Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IBaseScene, Object);

        virtual void                    SetRoot             (IGameObject * _sector) = 0;
        virtual IGameObject *           GetRoot             () const  = 0;

        virtual IWorld *                GetWorld            () const = 0;
        virtual BaseSceneType           GetSceneType        () const = 0;

        //virtual void                    SetSceneRenderData  (IBaseSceneRenderData * _sceneRenderData) = 0;
        virtual IBaseSceneRenderData *  GetSceneRenderData  () const = 0;
    };
}