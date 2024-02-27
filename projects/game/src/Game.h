#pragma once

#include "application/IProject.h"
#include "core/Singleton/Singleton.h"
#include "game_consts.h"

namespace vg::core
{
    class IInput;
}

namespace vg::engine
{
    class IEngine;
}

class CharacterBehaviour;
class PlayerBehaviour;
class EnemyBehaviour;

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

        void                                            addCharacter            (CharacterType _type, CharacterBehaviour * _character);
        void                                            removeCharacter         (CharacterType _type, CharacterBehaviour * _character);
        const vg::core::vector<CharacterBehaviour *> &  getCharacters           (CharacterType _type) const { return m_characters[vg::core::asInteger(_type)];}

        const vg::core::vector<PlayerBehaviour *> &     getPlayers              () const;
        const vg::core::vector<EnemyBehaviour *> &      getEnemies              () const;

    private:
        static vg::engine::IEngine *                    s_engine;

        vg::core::vector<CharacterBehaviour *>          m_characters[vg::core::enumCount<CharacterType>()];
};