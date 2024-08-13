#pragma once
#include "core/IBehaviour.h"

namespace vg::core
{
    class Behaviour : public IBehaviour
    {
    public:
        VG_CLASS_DECL(Behaviour, IBehaviour);

        Behaviour(const core::string & _name, IObject * _parent);
        ~Behaviour();

        void    OnCollisionEnter    (IGameObject * _other) override;
        void    OnCollisionStay     (IGameObject * _other) override;
        void    OnCollisionExit     (IGameObject * _other) override;
                                                                 
        void    OnTriggerEnter      (IGameObject * _other) override;
        void    OnTriggerStay       (IGameObject * _other) override;
        void    OnTriggerExit       (IGameObject * _other) override;
    };
}