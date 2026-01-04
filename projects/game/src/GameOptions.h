#pragma once
#include "core/Singleton/Singleton.h"
#include "application/IGameOptions.h"

class GameOptions final : public vg::IGameOptions, public vg::core::Singleton<GameOptions>
{
public:
    VG_CLASS_DECL(GameOptions, IGameOptions);

    GameOptions(const vg::core::string & _name, vg::core::IObject * _parent = nullptr);
    ~GameOptions();

    VG_INLINE float getDeathHeight() const { return m_deathHeight; }
    VG_INLINE float getRespawnHeight() const { return m_respawnHeight; }

private:
    float   m_deathHeight = -32.0f;
    float   m_respawnHeight = 16.0f;
};