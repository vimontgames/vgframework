#include "ChestBehaviour.h"
#include "Game.h"
#include "Behaviour/Entity/Item/ItemBehaviour.h"
#include "Behaviour/Entity/Character/CharacterBehaviour.h"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(ChestBehaviour, "Chest", "Game", "A chest the player can destroy", vg::editor::style::icon::Script, 0);

//--------------------------------------------------------------------------------------
ChestBehaviour::ChestBehaviour(const string & _name, IObject * _parent) :
    super(_name, _parent, ItemType::Chest)
{
    EnableUpdateFlags(vg::core::UpdateFlags::Update, false);
}

//--------------------------------------------------------------------------------------
ChestBehaviour::~ChestBehaviour()
{

}

//--------------------------------------------------------------------------------------
bool ChestBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    registerPropertyEnum(ChestBehaviour, ChestType, m_chestType, "Type");
    setPropertyDescription(ChestBehaviour, m_chestType, "Chest type");

    return true;
}

//--------------------------------------------------------------------------------------
void ChestBehaviour::OnPlay()
{
    super::OnPlay();
}

//--------------------------------------------------------------------------------------
bool ChestBehaviour::TakeHit(CharacterBehaviour * _attacker, ItemBehaviour * _weapon)
{
    if (1)
    {
        auto * go = GetGameObject();
        float3 hitDir;

        if (nullptr != _attacker)
        {
            auto * attackerGO = _attacker->GetGameObject();
            hitDir = normalize(go->GetGlobalMatrix()[3].xyz - attackerGO->GetGlobalMatrix()[3].xyz);

            if (_weapon)
                VG_INFO("[Entity] \"%s\" was hit by \"%s\" with \"%s\"", go->GetName().c_str(), attackerGO->GetName().c_str(), _weapon->GetGameObject()->GetName().c_str());
            else
                VG_INFO("[Entity] \"%s\" was hit by \"%s\"", go->GetName().c_str(), attackerGO->GetName().c_str());

            go->SetColor(float4(0, 1, 0, 1));
        }
        else
        {
            auto * weaponGO = _weapon->GetGameObject();
            hitDir = normalize(go->GetGlobalMatrix()[3].xyz - weaponGO->GetGlobalMatrix()[3].xyz);

            VG_INFO("[Entity] \"%s\" was hit by \"%s\"", go->GetName().c_str(), weaponGO->GetName().c_str());
        }

        return true; // hit was taken
    }

    return false;
}



