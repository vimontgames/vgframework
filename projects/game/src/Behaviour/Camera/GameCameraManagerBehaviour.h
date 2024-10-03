#include "core/Component/Behaviour/Behaviour.h"
#include "core/Object/ObjectHandle.h"

class GameCameraManagerBehaviour final : public vg::core::Behaviour
{
public:
    VG_CLASS_DECL(GameCameraManagerBehaviour, vg::core::Behaviour);

    GameCameraManagerBehaviour(const vg::core::string & _name, vg::core::IObject * _parent);
    ~GameCameraManagerBehaviour();

    void                    OnPlay() final override;
    void                    Update(const Context & _context) final override;
    void                    OnPropertyChanged(vg::core::IObject * _object, const vg::core::IProperty & _prop, bool _notifyParent) final override;

private:
    void                    updateGameViewports();

private:
    vg::core::uint          m_players = 0;

    vg::core::ObjectHandle  m_player1Camera;
    vg::core::ObjectHandle  m_player2Camera;
    vg::core::ObjectHandle  m_player3Camera;
    vg::core::ObjectHandle  m_player4Camera;
};