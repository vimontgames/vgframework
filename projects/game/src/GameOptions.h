#pragma once
#include "core/Singleton/Singleton.h"
#include "application/IGameOptions.h"

class GameOptions final : public vg::IGameOptions, public vg::core::Singleton<GameOptions>
{
public:
    VG_CLASS_DECL(GameOptions, IGameOptions);

    GameOptions(const vg::core::string & _name, vg::core::IObject * _parent = nullptr);
    ~GameOptions();

    VG_INLINE float getMinimumHeight() const { return m_minZ; }

private:
    float   m_minZ = -32.0f;
};