#pragma once

#include "application/IGame.h"
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

class ItemBehaviour;
class BallBehaviour;
class WeaponBehaviour;

class Game : public vg::IGame, public vg::core::Singleton<Game>
{
    public:
        VG_CLASS_DECL(Game, vg::IGame);
         Game        ();
         ~Game       ();

        vg::core::IPlugin::Version                      GetVersion              () const final override;
        bool                                            RegisterClasses         () final override;
        bool                                            UnregisterClasses       ();

        bool                                            Init                    (vg::engine::IEngine & _engine, vg::core::Singletons & _singletons) final override;
        bool                                            Deinit                  () final;

        void                                            Update                  (float _dt) final override;

        static vg::engine::IEngine &                    Engine                  ();
        static vg::core::IInput &                       Input                   ();

        void                                            addCharacter            (CharacterType _type, CharacterBehaviour * _character);
        void                                            removeCharacter         (CharacterType _type, CharacterBehaviour * _character);
        const vg::core::vector<CharacterBehaviour *> &  getCharacters           (CharacterType _type) const { return m_characters[vg::core::asInteger(_type)];}

        const vg::core::vector<PlayerBehaviour *> &     getPlayers              () const;
        const vg::core::vector<PlayerBehaviour*>        getActivePlayers        () const;
        const vg::core::vector<EnemyBehaviour *> &      getEnemies              () const;

        void                                            addItem                 (ItemType _type, ItemBehaviour * _item);
        void                                            removeItem              (ItemType _type, ItemBehaviour * _item);
        const vg::core::vector<ItemBehaviour *> &       getItem                 (ItemType _type) const { return m_items[vg::core::asInteger(_type)]; }

        const vg::core::vector<BallBehaviour *> &       getBalls                () const;
        const vg::core::vector<WeaponBehaviour*> &      getWeapons              () const;

    private:
        static vg::engine::IEngine *                    s_engine;

        vg::core::vector<CharacterBehaviour *>          m_characters[vg::core::enumCount<CharacterType>()];
        vg::core::vector<ItemBehaviour *>               m_items[vg::core::enumCount<ItemType>()];
};