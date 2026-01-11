#pragma once
#include "Game_consts.h"
#include "Behaviour/Entity/Item/ItemBehaviour.h"

class VehicleBehaviour : public ItemBehaviour
{
public:
    VG_CLASS_DECL(VehicleBehaviour, ItemBehaviour);

    VehicleBehaviour(const vg::core::string & _name, vg::core::IObject * _parent, VehicleType _vehicleType = VehicleType::Default);
    ~VehicleBehaviour();

    void OnCollisionEnter(vg::core::IGameObject * _other) override;

    float GetDamage() const override;
    float GetPush() const override;

private:
    VehicleType m_vehicleType = VehicleType::Default;
    float m_velocityThreshold = 2.0f; 
};

#if VG_ENABLE_INLINE
#include "VehicleBehaviour.inl"
#endif
