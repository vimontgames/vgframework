#pragma once
#include "core/Component/Component.h"

class GameCameraComponent : public vg::core::Component
{
public:
    VG_CLASS_DECL(GameCameraComponent, vg::core::Component);

    GameCameraComponent(const vg::core::string & _name, vg::core::IObject * _parent);
    ~GameCameraComponent();

    void            OnPlay      () final override;
    void            Update      (float _dt) override;

private:
    float           m_speed = 1.0f;
};
