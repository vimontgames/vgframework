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

        registerOptionalPropertyEnumBitfield(PhysicsOptions, m_showMotionTypes, MotionTypeFlags, m_showMotionTypesMask, "Motion");
        setPropertyDescription(PhysicsOptions, m_showMotionTypesMask, "Show physics bodies using motion type");

        registerOptionalPropertyEnumBitfield(PhysicsOptions, m_showRigidBodies, ShapeTypeFlags, m_showRigidBodiesMask, "Body");
        setPropertyDescription(PhysicsOptions, m_showRigidBodiesMask, "Show physics bodies of type");

        registerOptionalPropertyEnumBitfield(PhysicsOptions, m_showConstraints, ConstraintTypeFlags, m_showConstraintsMask, "Contraints");
        setPropertyDescription(PhysicsOptions, m_showConstraintsMask, "Show physics constraints of type");

        registerProperty(PhysicsOptions, m_mergeStaticBodies, "Merge static");
        setPropertyDescription(PhysicsOptions, m_mergeStaticBodies, "Merge static bodies at init");

        registerProperty(PhysicsOptions, m_gravity, "Gravity");
        setPropertyDescription(PhysicsOptions, m_gravity, "Gravity force applied to physic objects");

        registerPropertyEnumArray(PhysicsOptions, string, Category, m_physicsCategories, "Category");
        setPropertyDescription(PhysicsOptions, m_physicsCategories, "Use physics Categories to filter collisions")

        registerPropertyOptionalGroupBegin(PhysicsOptions, m_debugRendererEnable, "Debug Renderer");
        {
            registerProperty(PhysicsOptions, m_debugRendererDrawShape, "Shapes");
            setPropertyDescription(PhysicsOptions, m_debugRendererDrawShape, "Draw shapes");

            registerProperty(PhysicsOptions, m_debugRendererDrawShapeWireframe, "Shapes Wireframe");
            setPropertyDescription(PhysicsOptions, m_debugRendererDrawShapeWireframe, "Use wreframe to draw shapes");
            
            registerProperty(PhysicsOptions, m_debugRendererDrawVelocity, "Velocity");
            setPropertyDescription(PhysicsOptions, m_debugRendererDrawVelocity, "Draw velocity vectors");

            registerProperty(PhysicsOptions, m_debugRendererContraints, "Constraints");
            setPropertyDescription(PhysicsOptions, m_debugRendererContraints, "Draw constraints");
        }
        registerPropertyOptionalGroupEnd(PhysicsOptions);
        
        return true;
    }

    //--------------------------------------------------------------------------------------
    PhysicsOptions::PhysicsOptions(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {
        SetFile("Physics.xml");
        Load(false);

        if (auto * physicsCategoriesProp = GetClassDesc()->GetPropertyByName(VG_STRINGIFY(m_physicsCategories)))
            updateDynamicEnum(*physicsCategoriesProp);
    }

    //--------------------------------------------------------------------------------------
    void PhysicsOptions::OnLoad()
    {
        if (auto gravityProp = GetClassDesc()->GetPropertyByName("m_gravity"))
            applyGravity(*gravityProp->GetPropertyFloat3(this));
    }

    //--------------------------------------------------------------------------------------
    void PhysicsOptions::OnPropertyChanged(IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        const char * name = _prop.GetName();

        if (!strcmp(name, "m_gravity"))
            applyGravity(*_prop.GetPropertyFloat3(_object));

        updateDynamicEnum(_prop);
    }

    //--------------------------------------------------------------------------------------
    // Patch properties using a generic enum type
    //--------------------------------------------------------------------------------------
    void PhysicsOptions::updateDynamicEnum(const core::IProperty & _prop)
    {

        IFactory * factory = Kernel::getFactory();

        const auto classDescs = factory->GetClassDescriptors();

        for (auto * desc : classDescs)
        {
            const auto propCount = desc->GetPropertyCount();
            for (uint i = 0; i < propCount; ++i)
            {
                IProperty * prop = const_cast<IProperty*>(desc->GetPropertyByIndex(i)); // TODO: non-const version?

                if (!strcmp(_prop.GetName(), VG_STRINGIFY(m_physicsCategories)))
                {
                    if ((PropertyType::EnumU8 == prop->GetType() && !strcmp(VG_STRINGIFY(Category), prop->GetEnumTypeName()))
                        || (PropertyType::EnumFlagsU64 == prop->GetType() && !strcmp(VG_STRINGIFY(CategoryFlag), prop->GetEnumTypeName())))
                    {
                        for (uint i = 0; i < countof(m_physicsCategories); ++i)
                            prop->SetEnumName(i, m_physicsCategories[i]);
                    }
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsOptions::IsAnyBodyVisible() const 
    {
        return 0 != m_showRigidBodies; 
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsOptions::IsBodyVisible(physics::ShapeType _shape, physics::MotionType _motionType) const 
    {
        return (m_showRigidBodies && ((1 << (core::u32)_shape) & (core::u32)m_showRigidBodiesMask)) && (m_showMotionTypes && ((1 << (core::u32)_motionType) & (core::u32)m_showMotionTypesMask));
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsOptions::IsConstraintVisible(ConstraintType _constraint) const
    {
        return (m_showConstraints && ((1 << (core::u32)_constraint) & (core::u32)m_showConstraintsMask));
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsOptions::IsMergeStaticBodiesEnabled() const 
    {
        return m_mergeStaticBodies;
    }

    //--------------------------------------------------------------------------------------
    Category PhysicsOptions::GetPhysicsCategory(const core::string & _name) const
    {
        for (uint i = 0; i < countof(m_physicsCategories); ++i)
        {
            if (_name == m_physicsCategories[i])
                return (Category)i;
        }

        VG_WARNING("[Physics] Could not find physics::Category with name \"%s\"", _name.c_str());
        return (Category)0;
    }

    //--------------------------------------------------------------------------------------
    core::vector<core::string> PhysicsOptions::GetPhysicsCategoryNames() const
    {
        vector<string> names(enumCount<physics::Category>());
        for (uint i = 0; i < names.size(); ++i)
            names[i] = m_physicsCategories[i];
        return names;
    }

    //--------------------------------------------------------------------------------------
    void PhysicsOptions::applyGravity(const core::float3 & _gravity)
    {
        Physics::get()->SetGravity(_gravity);
    }
}