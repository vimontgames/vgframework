#pragma once

#include "engine/IPhysicsShapeComponent.h"
#include "physics/Physics_Consts.h"

namespace vg::physics
{
    class IPhysics;
    class IShape;
    class IShapeDesc;
}

namespace vg::engine
{
    class PhysicsBodyComponent;

    class PhysicsShapeComponent final : public IPhysicsShapeComponent
    {
    public:
        VG_CLASS_DECL(PhysicsShapeComponent, IPhysicsShapeComponent);

        PhysicsShapeComponent(const core::string & _name, IObject * _parent);
        ~PhysicsShapeComponent();

        void                        OnLoad              () final override;
        void                        OnPropertyChanged   (core::IObject * _object, const core::IProperty & _prop, bool _notifyParent) final override;
        bool                        TryGetAABB          (core::AABB & _aabb) const final override;

        physics::IShape *           getPhysicsShape     () const { return m_shape; }
        physics::IShapeDesc *       getShapeDesc        () const { return m_shapeDesc; }
    
    private:
        PhysicsBodyComponent *      getBodyComponent    ();

        bool                        createShapeDesc     ();
        bool                        createShape         ();

        static physics::IPhysics *  getPhysics          ();

    private:
        physics::ShapeType          m_shapeType = physics::ShapeType::Box;
        physics::IShapeDesc *       m_shapeDesc = nullptr;
        physics::IShape *           m_shape = nullptr;
    };
}