#pragma once

#include "physics/IPhysics.h"
#include "core/Singleton/Singleton.h"
#include "physics/BroadPhase/BroadPhaseLayer.h"
#include "physics/BroadPhase/BroadPhaseFilter.h"
#include "physics/BroadPhase/ObjectFilter.h"

namespace vg::renderer
{
    class IDebugDraw;
}

namespace JPH
{
    class BodyID;
}

namespace vg::physics
{
    class JobSystemAdapter;
    class PhysicsWorld;

	class Physics final : public IPhysics, public core::Singleton<Physics>
	{
	public:
        using super = IPhysics;

                                                Physics		                (const core::string & _name, core::IObject * _parent);
                                                ~Physics                    ();

        const char *						    GetClassName		        () const final override { return "Physics"; }

        bool                                    RegisterClasses		        () final override;
        bool                                    UnregisterClasses	        () final override;

        static bool                             registerProperties	        (core::IClassDesc & _desc);

        void							        Init                        (const PhysicsCreationParams & _params, const Callbacks & _callbacks, core::Singletons & _singletons) final override;
        void							        Deinit                      () final override;

        void                                    Update                      (float _dt) final override;

        IPhysicsOptions *                       GetOptions                  () const final override;
        void                                    SetGravity                  (const core::float3 _gravity) final override;

        core::IPhysicsWorld *                   CreatePhysicsWorld          (const core::IWorld * _world) final override;
        IShape *                                CreateShape                 (const IShapeDesc * _shapeDesc) final override;
        IBody *                                 CreateBody                  (core::IPhysicsWorld * _physicsWorld, const IBodyDesc * _bodyDesc, IShape * _shape, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent) final override;
        IBody *                                 CreateBody                  (core::IPhysicsWorld * _physicsWorld, const IBodyDesc * _bodyDesc, const core::vector<ShapeInfo> & _shapes, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent) final override;
        ICharacter *                            CreateCharacter             (core::IPhysicsWorld * _physicsWorld, const ICharacterDesc * _characterDesc, IShape * _shape, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent) final override;
        ICharacter *                            CreateCharacter             (core::IPhysicsWorld * _physicsWorld, const ICharacterDesc * _characterDesc, const core::vector<IShape*> & _shapes, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent) final override;
        void                                    RemoveBody                  (core::IPhysicsWorld * _physicsWorld, IBody * _body) final override;
        void                                    SetDrawShapeCallback        (DrawShapeCallback _drawShape) final override;
        VG_INLINE DrawShapeCallback             getDrawShapeCallback        () const { return m_drawShapeCallback; }

        engine::IEngine *                       getEngine                   () const;
        renderer::IDebugDraw *                  getDebugDraw                () const;

        VG_INLINE const PhysicsCreationParams & getPhysicsCreationParams    () const;
        VG_INLINE const Callbacks &             getCallbacks                () const;

        VG_INLINE BroadPhaseLayer &             getBroadPhaseLayer          ();
        VG_INLINE BroadPhaseFilter &            getBroadPhaseFilter         ();
        VG_INLINE ObjectFilter &                getObjectfilter             ();

        void                                    registerPhysicsWorld        (PhysicsWorld * _physicsWorld);
        void                                    unregisterPhysicsWorld      (PhysicsWorld * _physicsWorld);

    private:
        PhysicsCreationParams                   m_physicsCreationParams;
        Callbacks                               m_callbacks;
        JobSystemAdapter *                      m_jobSystem = nullptr;
        BroadPhaseLayer                         m_broadPhaseLayer;
        BroadPhaseFilter                        m_broadPhaseFilter;
        ObjectFilter                            m_objectFilter;
        JPH::TempAllocatorMalloc                m_tempAllocator;
        core::vector<PhysicsWorld*>             m_physicsWorlds;
        DrawShapeCallback                       m_drawShapeCallback = nullptr;
	};
}

#if VG_ENABLE_INLINE
#include "Physics.inl"
#endif