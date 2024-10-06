#include "WeaponBehaviour.h"
#include "editor/Editor_Consts.h"
#include "Game.h"
#include "Behaviour/Character/Player/PlayerBehaviour.h"
#include "engine/IAttachToNodeComponent.h"
#include "engine/IEngineOptions.h"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(WeaponBehaviour, "Weapon", "Game", "A weapon the player can pick", vg::editor::style::icon::Script, 0);

//--------------------------------------------------------------------------------------
WeaponBehaviour::WeaponBehaviour(const string & _name, IObject * _parent) :
    super(_name, _parent, ItemType::Weapon)
{
    EnableUpdateFlags(vg::core::UpdateFlags::Update, false);
}

//--------------------------------------------------------------------------------------
WeaponBehaviour::~WeaponBehaviour()
{

}

//--------------------------------------------------------------------------------------
bool WeaponBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    return true;
}

//--------------------------------------------------------------------------------------
void WeaponBehaviour::OnPlay()
{
    super::OnPlay();
    m_ennemyTag = Game::get()->Engine().GetOptions()->GetGameObjectTag("Enemy");
}

//--------------------------------------------------------------------------------------
void WeaponBehaviour::OnTriggerEnter(vg::core::IGameObject * _other)
{
    if (asBool(m_ennemyTag & _other->GetTags()))
    {
        if (auto * owner = GetOwner().get<IGameObject>())
        {
            VG_WARNING("[Enemy] Enemy \"%s\" was hit y Weapon \"%s\" hit owned by \"%s\" ", _other->GetName().c_str(), GetName().c_str(), owner->GetName().c_str());
        }
    }
}

//--------------------------------------------------------------------------------------
void WeaponBehaviour::Update(const Context & _context)
{

}

//--------------------------------------------------------------------------------------
void WeaponBehaviour::SetOwner(vg::core::IGameObject * _object)
{
    super::SetOwner(_object);

    if (auto * attachToNode = GetGameObject()->GetComponentT<vg::engine::IAttachToNodeComponent>())
    {
        attachToNode->SetUseTarget(true);
        attachToNode->SetTarget(_object);
    }
}


