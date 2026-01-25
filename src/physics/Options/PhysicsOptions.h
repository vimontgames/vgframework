#pragma once
#include "core/Singleton/Singleton.h"
#include "physics/IPhysicsOptions.h"

namespace vg::physics
{
    class PhysicsOptions final : public IPhysicsOptions, public core::Singleton<PhysicsOptions>
    {
    public:
        VG_CLASS_DECL(PhysicsOptions, IPhysicsOptions);

        PhysicsOptions(const core::string & _name, core::IObject * _parent = nullptr);

        void                        OnPropertyChanged               (core::IObject * _object, const core::IProperty & _prop, bool _notifyParent) final override;
        void                        OnLoad                          () final override;

        bool                        IsAnyBodyVisible                () const final override;
        bool                        IsBodyVisible                   (ShapeType _shape, MotionType _motionType) const final override;
        bool                        IsConstraintVisible             (ConstraintType _constraint) const final override;

        bool                        IsMergeStaticBodiesEnabled      () const final override;

        Category                    GetPhysicsCategory              (const core::string & _name) const final override;
        const core::vector<core::string>  GetPhysicsCategoryNames         () const final override;

        bool                        isDebugRendererEnabled          () const { return m_debugRendererEnable; }
        bool                        isDebugRendererShape            () const { return m_debugRendererDrawShape; }
        bool                        isDebugRendererShapeWireframe   () const { return m_debugRendererDrawShapeWireframe; }
        bool                        isDebugRendererVelocity         () const { return m_debugRendererDrawVelocity; }
        bool                        isDebugRendererConstraints      () const { return m_debugRendererContraints; }

    private:
        void                        applyGravity                    (const core::float3 & _gravity);
        void                        updateDynamicEnum               (const core::IProperty & _prop);

    private:
        // Engine
        bool                        m_showMotionTypes       = true;
        MotionTypeFlags             m_showMotionTypesMask   = (MotionTypeFlags)-1;
        bool                        m_showRigidBodies       = false;
        ShapeTypeFlags              m_showRigidBodiesMask   = (ShapeTypeFlags)-1;
        bool                        m_showConstraints       = false;
        ConstraintTypeFlags         m_showConstraintsMask   = (ConstraintTypeFlags)-1;
        core::float3		        m_gravity               = core::float3(0.0f, 0.0f, -9.81f);
        bool                        m_mergeStaticBodies     = false;
        vg::core::string            m_physicsCategories[core::enumCount<Category>()];

        // Debug renderer
        bool                        m_debugRendererEnable               = false;
        bool                        m_debugRendererDrawShape            = true;
        bool                        m_debugRendererDrawShapeWireframe   = true;      
        bool                        m_debugRendererDrawVelocity         = false;
        bool                        m_debugRendererContraints           = false;
    };
}