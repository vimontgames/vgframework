#pragma once
#include "core/Component/Component.h"

namespace vg::core
{
    class IBehaviour : public Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IBehaviour, Component);

        virtual void    OnCollisionEnter    (IGameObject * _other) = 0;
        virtual void    OnCollisionStay     (IGameObject * _other) = 0;
        virtual void    OnCollisionExit     (IGameObject * _other) = 0;

        virtual void    OnTriggerEnter      (IGameObject * _other) = 0;
        virtual void    OnTriggerStay       (IGameObject * _other) = 0;
        virtual void    OnTriggerExit       (IGameObject * _other) = 0;
    };
}