#pragma once

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // Parent should have the union of its children update flags (e.g. a GameObject should 'OR' the flags of all its children)
    //--------------------------------------------------------------------------------------
    vg_enum_class(UpdateFlags, u8,
        FixedUpdate     = 0x01, // 'FixedUpdate' should be used for physics-based calculations such as applying forces to Rigidbodies or any other calculations needing to be framerate independent
        Update          = 0x02, // 'Update' will be called once per frame
        LateUpdate      = 0x04  // 'LateUpdate' will also be called once per frame, after all other 'Updates' and should be used to e.g. animate model bones for lookup 
    );

    class IWorld;
    class IGameObject;

    struct GameObjectUpdateContext
    {
        GameObjectUpdateContext(bool _playing, bool _paused, float _dt, IWorld * _world) :
            m_playing(_playing),
            m_paused(_paused),
            m_dt(_dt),
            m_world(_world)
        {

        }

        bool            m_playing : 1;
        bool            m_paused : 1;
        float           m_dt;
        IWorld *        m_world;
    };

    struct ComponentUpdateContext : public GameObjectUpdateContext
    {
        ComponentUpdateContext(const GameObjectUpdateContext & _context, IGameObject * _gameObject) :
            GameObjectUpdateContext(_context),
            m_gameObject(_gameObject)
        {
   
        }

        IGameObject *   m_gameObject;
    };
}