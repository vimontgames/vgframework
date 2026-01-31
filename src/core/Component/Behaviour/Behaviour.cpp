#include "core/Precomp.h"
#include "Behaviour.h"
#include "core/Object/AutoRegisterClass.h"

namespace vg::core
{
    VG_REGISTER_ABSTRACT_CLASS(Behaviour);

    //--------------------------------------------------------------------------------------
    bool Behaviour::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    Behaviour::Behaviour(const core::string & _name, IObject * _parent) :
        IBehaviour(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    Behaviour::~Behaviour()
    {

    }

    //--------------------------------------------------------------------------------------
    void Behaviour::OnCollisionEnter(IGameObject * _other)
    {

    }

    //--------------------------------------------------------------------------------------
    void Behaviour::OnCollisionStay(IGameObject * _other)
    {

    }

    //--------------------------------------------------------------------------------------
    void Behaviour::OnCollisionExit(IGameObject * _other)
    {

    }
  
    //--------------------------------------------------------------------------------------
    void Behaviour::OnTriggerEnter(IGameObject * _other)
    {

    }

    //--------------------------------------------------------------------------------------
    void Behaviour::OnTriggerStay(IGameObject * _other) 
    {
    }

    //--------------------------------------------------------------------------------------
    void Behaviour::OnTriggerExit(IGameObject * _other)
    {

    }
}