#pragma once

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // Parent should have the union of its children update flags (e.g., a GameObject should 'OR' the flags of all its children)
    //--------------------------------------------------------------------------------------
    vg_enum_class(vg::core, UpdateFlags, u8,
        FixedUpdate     = 0x01, // 'FixedUpdate' should be used for physics-based calculations such as applying forces to Rigidbodies or any other calculations needing to be framerate independent
        Update          = 0x02, // 'Update' will be called once per frame
        LateUpdate      = 0x04, // 'LateUpdate' will also be called once per frame, after all other 'Updates' and should be used to e.g., animate model bones for lookup 
        ToolUpdate      = 0x08  // 'ToolUpdate' is used for editor/toolmode debug display and is only executed when the world has at least one toolmode view visible
    );

    class IWorld;
    class IGameObject;

    struct WorldUpdateContext
    {
        WorldUpdateContext(bool _playing, bool _paused, float _dt) :
            m_playing(_playing),
            m_paused(_paused),
            m_dt(_dt)
        {

        }

        bool            m_playing : 1;
        bool            m_paused : 1;
        float           m_dt;
    };

    struct GameObjectUpdateContext : public WorldUpdateContext
    {
        GameObjectUpdateContext(const WorldUpdateContext & _context, IWorld * _world) :
            WorldUpdateContext(_context),
            m_world(_world)
        {

        }
        
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