#pragma once

#include "application/IProject.h"
#include "core/Singleton/Singleton.h"

namespace vg::core
{
    class IInput;
}

namespace vg::engine
{
    class IEngine;
}

class PlayerBehaviour;

class Game : public vg::IProject, public vg::core::Singleton<Game>
{
    public:
        VG_CLASS_DECL(Game, vg::IProject);
         Game        ();
         ~Game       ();

        // vg::core::IPlugin overrides
        vg::core::IPlugin::Version                      GetVersion              () const final override;
        bool                                            RegisterClasses         () final override;
        bool                                            UnregisterClasses       ();

        // vg::core::IProject overrides
        bool                                            init                    (vg::engine::IEngine & _engine, vg::core::Singletons & _singletons) final;
        bool                                            deinit                  () final;

        bool                                            update                  () final;

        static vg::engine::IEngine &                    Engine                  ();
        static vg::core::IInput &                       Input                   ();

        void                                            addPlayer               (PlayerBehaviour * _player);
        void                                            removePlayer            (PlayerBehaviour * _player);
        const vg::core::vector<PlayerBehaviour *> &     getPlayers              () const { return m_players;}

    private:
        static vg::engine::IEngine *                    s_engine;

        vg::core::vector<PlayerBehaviour *>             m_players;
};