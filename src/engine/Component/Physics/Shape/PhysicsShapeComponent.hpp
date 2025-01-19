#include "PhysicsShapeComponent.h"
#include "core/IGameObject.h"
#include "core/IWorld.h"
#include "engine/Engine.h"
#include "engine/EngineOptions.h"
#include "engine/Resource/Mesh/MeshResource.h"
#include "renderer/IRenderer.h"
#include "renderer/IMeshModel.h"
#include "renderer/IDebugDraw.h"
#include "physics/IShape.h"
#include "editor/Editor_Consts.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(PhysicsShapeComponent, "Physics Shape", "Physics", "A shape to use with physics body", editor::style::icon::PhysicsShape, 0);

    //--------------------------------------------------------------------------------------
    bool PhysicsShapeComponent::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnumEx(PhysicsShapeComponent, physics::ShapeType, m_shapeType, "Type", PropertyFlags::None);
        setPropertyDescription(PhysicsShapeComponent, m_shapeType, "Type of primitive to use for the physics shape");

        registerPropertyObjectPtrEx(PhysicsShapeComponent, m_shapeDesc, "Shape", PropertyFlags::Flatten);

        return true;
    }

    //--------------------------------------------------------------------------------------
    PhysicsShapeComponent::PhysicsShapeComponent(const core::string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        EnableUpdateFlags(UpdateFlags::Update, false);

        if (nullptr == m_shapeDesc)
            createShapeDesc();

        //if (nullptr == m_shape)
        //    createShape();
    }

    //--------------------------------------------------------------------------------------
    PhysicsShapeComponent::~PhysicsShapeComponent()
    {
        VG_SAFE_RELEASE(m_shape);
        VG_SAFE_RELEASE(m_shapeDesc);
    }

    //--------------------------------------------------------------------------------------
    PhysicsBodyComponent * PhysicsShapeComponent::getBodyComponent()
    {
        return GetGameObject()->GetComponentT<PhysicsBodyComponent>();
    }

    //--------------------------------------------------------------------------------------
    void PhysicsShapeComponent::OnLoad()
    {
        if (m_shapeDesc == nullptr)
            createShapeDesc();

        if (m_shape == nullptr)
            createShape();
    }

    //--------------------------------------------------------------------------------------
    void PhysicsShapeComponent::OnPropertyChanged(IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        if (!strcmp(_prop.GetName(), "m_shapeType") || strstr(_prop.GetClassName(), "ShapeDesc"))
        {
            createShapeDesc();
            createShape();
        }

        if (auto * bodyComp = getBodyComponent())
            bodyComp->onShapeUpdated();

        super::OnPropertyChanged(_object, _prop, _notifyParent);
    }


    //--------------------------------------------------------------------------------------
    void PhysicsShapeComponent::OnResourceLoaded(core::IResource * _resource)
    {
        if (m_shape)
        {
            if (auto * triangles = getColliderTriangles(m_shape))
            {
                m_shape->OnGeometryLoaded(*triangles);

                if (auto * bodyComp = getBodyComponent())
                    bodyComp->onShapeUpdated();
            }
            else
            {
                VG_WARNING("[Physics] Mesh \"%s\" has no collision data. Please check \"Collision data\" in mesh metadata", _resource->GetResourcePath().c_str());
            }
        }
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsShapeComponent::createShapeDesc()
    {
        IFactory * factory = Kernel::getFactory();

        if (nullptr == m_shapeDesc || m_shapeDesc->GetShapeType() != m_shapeType)
        {
            UID previousOriginalUID = 0;
            if (nullptr != m_shapeDesc)
                previousOriginalUID = m_shapeDesc->GetOriginalUID(false);

            VG_SAFE_RELEASE(m_shapeDesc);

            switch (m_shapeType)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(m_shapeType);
                    break;

                case physics::ShapeType::Sphere:
                    m_shapeDesc = (physics::IShapeDesc *)factory->CreateObject("SphereShapeDesc", "", this);
                    break;

                case physics::ShapeType::Box:
                    m_shapeDesc = (physics::IShapeDesc *)factory->CreateObject("BoxShapeDesc", "", this);
                    break;

                case physics::ShapeType::Capsule:
                    m_shapeDesc = (physics::IShapeDesc *)factory->CreateObject("CapsuleShapeDesc", "", this);
                    break;

                case physics::ShapeType::Cylinder:
                    m_shapeDesc = (physics::IShapeDesc *)factory->CreateObject("CylinderShapeDesc", "", this);
                    break;

                case physics::ShapeType::Mesh:
                    m_shapeDesc = (physics::IShapeDesc *)factory->CreateObject("MeshShapeDesc", "", this);
                    break;
            }

            if (m_shapeDesc)
            {
                VG_ASSERT(m_shapeDesc->GetShapeType() == m_shapeType);

                if (previousOriginalUID)
                    m_shapeDesc->SetOriginalUID(previousOriginalUID);
                else
                    m_shapeDesc->RegisterUID();
            }
        }

        return nullptr != m_shapeDesc;
    }

    //--------------------------------------------------------------------------------------
    physics::IPhysics * PhysicsShapeComponent::getPhysics()
    {
        return Engine::get()->GetPhysics();
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsShapeComponent::createShape()
    {
        VG_SAFE_RELEASE(m_shape);
        VG_ASSERT(m_shapeDesc);

        if (m_shapeDesc)
            m_shape = getPhysics()->CreateShape(m_shapeDesc);
        
        if (auto * bodyComp = getBodyComponent())
            m_shape->SetColor(bodyComp->getShapesColor());

        return nullptr != m_shape;
    }

    //--------------------------------------------------------------------------------------
    bool PhysicsShapeComponent::TryGetAABB(core::AABB & _aabb) const
    {
        if (m_shapeDesc)
        {
            if (m_shapeDesc->TryGetAABB(_aabb))
                return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    const core::vector<renderer::ColliderTriangle> * PhysicsShapeComponent::getColliderTriangles(const physics::IShape * _shape)
    {
        if (auto * res = _shape->GetResource())
        {
            const MeshResource * meshRes = VG_SAFE_STATIC_CAST(const MeshResource, res);
            if (nullptr != meshRes)
            {
                if (const renderer::IMeshModel * meshModel = meshRes->getMeshModel())
                {
                    renderer::IDebugDraw * debugDraw = Engine::get()->GetRenderer()->GetDebugDraw();
                    u32 color = _shape->GetColor();

                    const auto & triangles = meshModel->GetColliderTriangles();

                    if (triangles.size() > 0)
                        return &triangles;
                }
            }
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void PhysicsShapeComponent::drawShapeCallback(const physics::IShape * _shape, const core::IWorld * _world, const core::float4x4 & _matrix)
    {
        if (nullptr != _shape)
        {
            const auto shapeType = _shape->GetShapeType();
            switch (shapeType)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(shapeType);
                    break;

                case physics::ShapeType::Mesh:
                {
                    if (const core::vector<renderer::ColliderTriangle> * triangles = getColliderTriangles(_shape))
                    {
                        renderer::IDebugDraw * debugDraw = Engine::get()->GetRenderer()->GetDebugDraw();
                        u32 color = _shape->GetColor();

                        for (uint i = 0; i < triangles->size(); ++i)
                        {
                            const renderer::ColliderTriangle & tri = (*triangles)[i];

                            const float3 v0 = float3(tri.v[0].x, tri.v[0].y, tri.v[0].z);
                            const float3 v1 = float3(tri.v[1].x, tri.v[1].y, tri.v[1].z);
                            const float3 v2 = float3(tri.v[2].x, tri.v[2].y, tri.v[2].z);

                            // TODO: debugDraw->AddWireframeTriangle ?
                            debugDraw->AddLine(_world, v0, v1, color, _matrix);
                            debugDraw->AddLine(_world, v1, v2, color, _matrix);
                            debugDraw->AddLine(_world, v2, v0, color, _matrix);
                        }
                    }
                }
                break;
            }
        }
    }
}