#include "renderer/Precomp.h"
#include "core/Object/AutoRegisterClass.h"
#include "Geometry.h"

#include "gfx/Resource/Buffer.h"

#if !VG_ENABLE_INLINE
#include "Geometry.inl"
#endif

#include "Batch/Batch.hpp"
#include "AABB/AABB.hpp"
#include "Mesh/MeshGeometry.hpp"

namespace vg::renderer
{
    VG_REGISTER_OBJECT_CLASS(Geometry, "Geometry");

    //--------------------------------------------------------------------------------------
    bool Geometry::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        //_desc.registerPropertyEx(Geometry, m_boundingBox.m_min, "AABBMin", IProperty::Flags::ReadOnly);
        //_desc.registerPropertyEx(Geometry, m_boundingBox.m_max, "AABBMax", IProperty::Flags::ReadOnly);

        // TODO: registerSubPropertyEx("Geometry", m_aabb, m_min, "AABBMin", IProperty::Flags::ReadOnly)  ?
        _desc.RegisterProperty("Geometry", "m_min", (core::float3 *)(&((Geometry *)(nullptr))->m_aabb.m_min), "AABBMin", IProperty::Flags::ReadOnly);
        _desc.RegisterProperty("Geometry", "m_max", (core::float3 *)(&((Geometry *)(nullptr))->m_aabb.m_max), "AABBMax", IProperty::Flags::ReadOnly);

        return true;
    }
}
