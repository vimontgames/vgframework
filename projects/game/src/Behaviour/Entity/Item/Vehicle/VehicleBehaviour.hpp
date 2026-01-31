#include "Precomp.h"
#include "VehicleBehaviour.h"
#include "Game.h"
#include "GameOptions.h"
#include "Behaviour/Entity/Character/Player/PlayerBehaviour.h"

#if !VG_ENABLE_INLINE
#include "VehicleBehaviour.inl"
#endif

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(VehicleBehaviour, "Vehicle", "Game", "Game vehicle behaviour", vg::editor::style::icon::Script, getPriority(ComponentGroup::Behaviour, ComponentPriority::Late));

//--------------------------------------------------------------------------------------
bool VehicleBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    registerProperty(VehicleBehaviour, m_velocityThreshold, "Threshold");
    setPropertyDescription(VehicleBehaviour, m_velocityThreshold, "Damage will start when velocity is greater than this value (in m/s)")

    return true;
}

//--------------------------------------------------------------------------------------
VehicleBehaviour::VehicleBehaviour(const string & _name, IObject * _parent, VehicleType _vehicleType) :
    super(_name, _parent, ItemType::Vehicle),
    m_vehicleType(_vehicleType)
{
    EnableUpdateFlags(UpdateFlags::Update, false);
}

//--------------------------------------------------------------------------------------
VehicleBehaviour::~VehicleBehaviour()
{

}

//--------------------------------------------------------------------------------------
void VehicleBehaviour::OnCollisionEnter(IGameObject * _other)
{
    //VG_WARNING("[VehicleBehaviour] Vehicle \"%s\" entered collision with \"%s\"", GetGameObject()->GetFullName().c_str(), _other->GetFullName().c_str());

    if (auto * player = _other->GetComponentT<PlayerBehaviour>())
    {
        
    }
    else if (auto * enemy = _other->GetComponentInParentsT<EntityBehaviour>())
    {
        // check vehicle speed
        if (auto * physicsBodyComponent = GetGameObject()->GetComponentInChildrenT<IPhysicsBodyComponent>())
        {
            const float3 & velocity = physicsBodyComponent->GetLinearVelocity();
            const float velocityNorm = length(velocity);
            //VG_DEBUGPRINT("[ItemBehaviour] Velocity = %f, %f, %f (%f)\n", (float)velocity.x, (float)velocity.y, (float)velocity.z, velocityNorm);

            if (velocityNorm > 1.0f) // 1 m/s ==> 3.6 km/h
                enemy->TakeHit(GetOwnerCharacter(), this);
        }
    }
}

//--------------------------------------------------------------------------------------
// Make push and damage proportional to speed
//--------------------------------------------------------------------------------------
float VehicleBehaviour::GetDamage() const
{
    if (IPhysicsBodyComponent * body = GetGameObject()->GetComponentT<IPhysicsBodyComponent>())
    {
        const float velocity = length(body->GetLinearVelocity());
        float damage = max(0.0f,(velocity - m_velocityThreshold)) * super::GetDamage();
        //VG_WARNING("[Vehicle] damage = %f", damage); 
        return damage;
    }

    return super::GetDamage();
}

//--------------------------------------------------------------------------------------
// Make push and damage proportional to speed
//--------------------------------------------------------------------------------------
float VehicleBehaviour::GetPush() const
{
    if (IPhysicsBodyComponent * body = GetGameObject()->GetComponentT<IPhysicsBodyComponent>())
    {
        const float velocity = length(body->GetLinearVelocity());
        float push = max(0.0f,(velocity - m_velocityThreshold)) * super::GetPush();
        //VG_WARNING("[Vehicle] push = %f", push);
        return push;
    }

    return super::GetPush();
}
