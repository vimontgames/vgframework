#include "MeshShape.h"
#include "core/IResource.h"

using namespace vg::core;

#define USE_MESH_COLLIDER_POOL 1

#if USE_MESH_COLLIDER_POOL
struct SharedMeshShape
{
    JPH::MeshShape * shape = nullptr;
    float mass = 0.0f;
    u64 refCount = 1;
};

static map<u64, SharedMeshShape> g_meshColliderPool;
#endif

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    VG_REGISTER_CLASS(MeshShapeDesc, "Mesh Shape Settings");

    //--------------------------------------------------------------------------------------
    MeshShapeDesc::MeshShapeDesc(const vg::core::string & _name, vg::core::IObject * _parent) :
        super(_name, _parent)
    {
        m_meshResource = (IResource*)Kernel::getFactory()->CreateObject("MeshResource");
        m_meshResource->SetParent(this);
        m_meshResource->RegisterUID();
    }

    //--------------------------------------------------------------------------------------
    MeshShapeDesc::~MeshShapeDesc()
    {
        VG_SAFE_RELEASE(m_meshResource);
    }

    //--------------------------------------------------------------------------------------
    bool MeshShapeDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyResourcePtr(MeshShapeDesc, m_meshResource, "Mesh");
        setPropertyDescription(MeshShapeDesc, m_meshResource, "Mesh resource");

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MeshShapeDesc::TryGetAABB(AABB & _aabb) const
    {
        // TODO
        //_aabb = AABB(-m_size.xyz * 0.5f + m_offset, m_size.xyz * 0.5f + m_offset);
        return false;
    }

    //--------------------------------------------------------------------------------------
    VG_REGISTER_CLASS(MeshShape, "Mesh Shape");
    //--------------------------------------------------------------------------------------
    bool MeshShape::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }

    //--------------------------------------------------------------------------------------
    MeshShape::MeshShape(const MeshShapeDesc & _desc) :
        Shape("Mesh", nullptr),
        m_rotation(_desc.getRotation()),
        m_translation(_desc.getTranslation())
    {
        m_transform = _desc.getMatrix();
        m_meshResource = (IResource*)_desc.GetResource();
        VG_SAFE_INCREASE_REFCOUNT(m_meshResource);
    }

    //--------------------------------------------------------------------------------------
    MeshShape::~MeshShape()
    {
        #if USE_MESH_COLLIDER_POOL
        // Remove from pool
        if (m_shape)
        {
            const u64 key = hash<string>()(m_meshResource->GetResourcePath()) ^ m_meshResource->GetLastCookDate();
            if (0 != key)
            {
                auto it = g_meshColliderPool.find(key);

                if (it != g_meshColliderPool.end())
                {
                    SharedMeshShape & entry = it->second;
                    entry.refCount--;

                    if (entry.refCount == 0)
                    {
                        VG_INFO("[Physics] Release MeshShape \"%s\" with key 0x%016llx from pool", m_meshResource->GetResourcePath().c_str(), key);
                        VG_SAFE_RELEASE(entry.shape);
                        g_meshColliderPool.erase(it);
                    }
                    else
                    {
                        VG_INFO("[Physics] Decrease RefCount of MeshShape \"%s\" with key 0x%016llx from pool (%u remaining)", m_meshResource->GetResourcePath().c_str(), key, entry.refCount);
                    }
                }
            }
        }
        #endif

        VG_SAFE_RELEASE(m_meshResource);
    }

    //--------------------------------------------------------------------------------------
    float MeshShape::GetMass() const
    {
        return m_mass;
    }

    //--------------------------------------------------------------------------------------
    void MeshShape::OnGeometryLoaded(const core::vector<renderer::ColliderTriangle> & _triangles) 
    {
        VG_SAFE_RELEASE(m_shape);

#if USE_MESH_COLLIDER_POOL
        // Unique key from file name and last access time
        const u64 key = hash<string>()(m_meshResource->GetResourcePath()) ^ m_meshResource->GetLastCookDate();

        // use existing mesh or create it
        JPH::MeshShape * meshShape = nullptr;
        float mass = 0.0f;

        auto it = g_meshColliderPool.find(key);
        if (it != g_meshColliderPool.end())
        {
            SharedMeshShape & entry = it->second;
            meshShape = entry.shape;
            mass = entry.mass;
            entry.refCount++;
            VG_INFO("[Physics] Increase RefCount of MeshShape \"%s\" with key 0x%016llx from pool (%u remaining)", m_meshResource->GetResourcePath().c_str(), key, entry.refCount);
        }
        else
        {
            if (CreateMeshShape(_triangles, meshShape, mass))
            {
               SharedMeshShape entry;
               entry.shape = meshShape;
               entry.mass = mass;
               entry.refCount = 1;
               VG_INFO("[Physics] Creating shared MeshShape \"%s\" with key 0x%016llx", m_meshResource->GetResourcePath().c_str(), key);
               g_meshColliderPool.insert({ key, entry });
            }
        }

        #pragma push_macro("new")
        #undef new
        m_shape = new JPH::RotatedTranslatedShape(getJoltVec3(m_translation), getJoltQuaternion(m_rotation), meshShape);
        m_mass = mass;
        #pragma pop_macro("new")

        m_shape->AddRef();

#else

        JPH::MeshShape * meshShape = nullptr;
        float mass = 0.0f;

        if (CreateMeshShape(_triangles, meshShape, mass))
        {
            #pragma push_macro("new")
            #undef new
            m_shape = new JPH::RotatedTranslatedShape(getJoltVec3(m_translation), getJoltQuaternion(m_rotation), meshShape);
            m_mass = mass;
            #pragma pop_macro("new")

            // Intermediate shape should not be deleted after use (Will be deleted by the RotatedTranslatedShape m_shape?)
            VG_SAFE_RELEASE(meshShape);

            m_shape->AddRef();
        }
#endif
    }

    //--------------------------------------------------------------------------------------
    bool MeshShape::CreateMeshShape(const core::vector<renderer::ColliderTriangle> & _triangles, JPH::MeshShape *& _shape, float & _mass)
    {
        JPH::TriangleList triangleList;

        triangleList.resize(_triangles.size());
        float3 minPos = float3(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT);
        float3 maxPos = float3(-MAX_FLOAT, -MAX_FLOAT, -MAX_FLOAT);
        for (uint i = 0; i < _triangles.size(); ++i)
        {
            for (uint v = 0; v < 3; ++v)
            {
                const auto & src = _triangles[i].v[2 - v];
                auto & dst = triangleList[i].mV[v];

                dst.x = src.x;
                dst.y = src.y;
                dst.z = src.z;

                const float3 P = float3(src.x, src.y, src.z);
                minPos = min(minPos, P);
                maxPos = max(maxPos, P);
            }
            triangleList[i].mMaterialIndex = _triangles[i].matID;
        }

        // Jolt cannot compute mass for mesh shapes, then use the AABB as a quick estimate
        const float eps = 0.01f;
        float mass = max(float(maxPos.x - minPos.x), eps) * max(float(maxPos.y - minPos.y), eps) * max(float(maxPos.z - minPos.z), eps);

        JPH::MeshShapeSettings meshShapeSettings(triangleList);
        JPH::Shape::ShapeResult result;

        #pragma push_macro("new")
        #undef new
        auto shape = new JPH::MeshShape(meshShapeSettings, result);
        if (result.HasError())
            VG_WARNING("[Physics] %s", result.GetError().c_str());
        #pragma pop_macro("new")

        _shape = shape;
        _mass = mass;

        _shape->AddRef();

        return true;
    }

    //--------------------------------------------------------------------------------------
    void MeshShape::Draw(const core::IWorld * _world, const core::float4x4 & _matrix) const
    {
        auto drawCallback = Physics::get()->getDrawShapeCallback();
        if (nullptr != drawCallback)
            drawCallback(this, _world, mul(m_transform, clearScale(_matrix)));
    }
}