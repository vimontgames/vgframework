#pragma once

namespace vg::core
{
    class GameObject;
}

namespace vg::physics
{
    struct Contact
    {
        Contact(){}

        Contact(core::IGameObject * _obj1, core::IGameObject * _obj2) :
            m_obj1(_obj1),
            m_obj2(_obj2)
        {

        }

        core::IGameObject * m_obj1 = nullptr;
        core::IGameObject * m_obj2 = nullptr;
    };
}