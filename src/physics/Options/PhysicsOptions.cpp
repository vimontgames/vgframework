#include "physics/Precomp.h"
#include "PhysicsOptions.h"
#include "physics/Physics.h"

using namespace vg::core;

namespace vg::physics
{
    VG_REGISTER_OBJECT_CLASS(PhysicsOptions, "Physics Options");

    //--------------------------------------------------------------------------------------
    bool PhysicsOptions::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalPropertyEnumBitfield(PhysicsOptions, m_showRigidBodies, ShapeTypeFlags, m_showRigidBodiesMask, "RigidBody")

        registerProperty(PhysicsOptions, m_gravity, "Gravity");

        // TODO: Move to menu or toolbar instead
        registerPropertyCallback(PhysicsOptions, load, "Load");
        registerPropertyCallbackEx(PhysicsOptions, save, "Save", IProperty::Flags::SameLine);

        return true;
    }

    static const char * filename = "PhysicsOptions.xml";

    //--------------------------------------------------------------------------------------
    PhysicsOptions::PhysicsOptions(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {
        load(this);
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsOptions::load(IObject * _object)
    {
        const auto * factory = Kernel::getFactory();
        if (factory->loadFromXML(_object, filename))
        {
            auto * physicsOptions = static_cast<PhysicsOptions *>(_object);

            if (auto gravityProp = _object->getClassDesc()->GetPropertyByName("m_gravity"))
                physicsOptions->applyGravity(*gravityProp->GetPropertyFloat3(_object));

            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsOptions::save(IObject * _object)
    {
        const auto * factory = Kernel::getFactory();
        return factory->saveToXML(_object, filename);
    }

    //--------------------------------------------------------------------------------------
    void PhysicsOptions::OnPropertyChanged(IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        const char * name = _prop.getName();

        if (!strcmp(name, "m_gravity"))
            applyGravity(*_prop.GetPropertyFloat3(_object));
    }

    //--------------------------------------------------------------------------------------
    void PhysicsOptions::applyGravity(const core::float3 & _gravity)
    {
        Physics::get()->SetGravity(_gravity);
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsOptions::IsRigidBodyVisible(ShapeType _shape) const
    {
        return m_showRigidBodies && asBool(ShapeTypeFlags(1 << (u32)_shape) & m_showRigidBodiesMask);
    }
}