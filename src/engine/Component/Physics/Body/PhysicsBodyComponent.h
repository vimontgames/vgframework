#pragma once

#include "engine/IPhysicsBodyComponent.h"
#include "physics/Physics_Consts.h"

namespace vg::physics
{
    class IPhysics;
    class IBody;
    class IBodyDesc;
}

namespace vg::engine
{
    class PhysicsShapeComponent;

    class PhysicsBodyComponent final : public IPhysicsBodyComponent
    {
    public:
        VG_CLASS_DECL(PhysicsBodyComponent, IPhysicsBodyComponent);

        PhysicsBodyComponent(const core::string & _name, IObject * _parent);
        ~PhysicsBodyComponent();

        void                                    Update              (const Context & _context) final override;
        void                                    OnLoad              () final override;
        void                                    OnPlay              () final override;
        void                                    OnStop              () final override;
        void                                    OnPropertyChanged   (core::IObject * _object, const core::IProperty & _prop, bool _notifyParent) final override;
        bool                                    TryGetAABB          (core::AABB & _aabb) const final override;
        core::u32                               getShapesColor      () const;

        void                                    onShapeUpdated      ();
        void                                    updateShapesColor   ();

        core::vector<PhysicsShapeComponent *>   getShapes           () const;
        const physics::IBodyDesc *              getBodyDesc         () const { return m_bodyDesc; }

    private:
        bool                                    updateFlagsFromGameObject();

        bool                                    createBodyDesc();
        bool                                    createBody();

        static physics::IPhysics *              getPhysics();

    private:
        physics::IBodyDesc *                    m_bodyDesc  = nullptr;
        physics::IBody *                        m_body      = nullptr;
    };
}