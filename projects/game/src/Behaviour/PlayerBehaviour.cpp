#include "Precomp.h"
#include "PlayerBehaviour.h"
#include "Game.h"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(PlayerBehaviour, "PlayerBehaviour Component");

//--------------------------------------------------------------------------------------
PlayerBehaviour::PlayerBehaviour(const string & _name, IObject * _parent) :
    Component(_name, _parent)
{

}

//--------------------------------------------------------------------------------------
PlayerBehaviour::~PlayerBehaviour()
{

}

//--------------------------------------------------------------------------------------
bool PlayerBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    _desc.registerProperty(PlayerBehaviour, m_joypadID, "Joypad");
    
    return true;
}

//--------------------------------------------------------------------------------------
void PlayerBehaviour::Update(double _dt)
{
    if (Game::Engine().IsPlaying())
    {
        IInput & input = Game::Input();

        if (m_joypadID < input.GetJoyCount())
        {
            if (input.IsJoyButtonJustPressed(m_joypadID, JoyButton::A))
            {
                VG_INFO("Button 'A' pressed on joypad %u", m_joypadID);

                IAnimationComponent * animationComponent = GetGameObject()->GetComponentByType<IAnimationComponent>();
                if (animationComponent)
                {

                }
            }

            float3 translation = float3(0, 0, 0);
            const float joyDeadZone = 0.15f;
            const float speed = 0.001f; // 0.1 cm/s

            const float2 leftJoyDir = input.GetJoyLeftStickDir(m_joypadID);
            if (abs(leftJoyDir).x > joyDeadZone)
                translation.x += leftJoyDir.x * speed * _dt;

            if (abs(leftJoyDir).y > joyDeadZone)
                translation.y -= leftJoyDir.y * speed * _dt;

            const float2 rightJoyDir = input.GetJoyRightStickDir(m_joypadID);
            if (abs(rightJoyDir).y > joyDeadZone)
                translation.z -= rightJoyDir.y * speed * _dt;

            IGameObject * go = GetGameObject();
            float4x4 worldMatrix = go->GetWorldMatrix();
            {
                worldMatrix[3].xyz += translation;
            }
            go->setWorldMatrix(worldMatrix);
        }
        else
        {
            VG_DEBUGPRINT("[Player] %u is not a valid Joypad (%u joypads detected)", m_joypadID, input.GetJoyCount());
        }
    }
}