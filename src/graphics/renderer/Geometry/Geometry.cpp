#include "graphics/renderer/Precomp.h"
#include "core/Object/AutoRegisterClass.h"
#include "Geometry.h"

#include "gfx/Resource/Buffer.h"

#if !VG_ENABLE_INLINE
#include "Geometry.inl"
#endif

#include "Batch/Batch.hpp"
#include "AABB/AABB.hpp"
#include "Mesh/MeshGeometry.hpp"

namespace vg::graphics::renderer
{
    VG_AUTO_REGISTER_CLASS(Geometry);

    //--------------------------------------------------------------------------------------
    bool Geometry::registerClass(IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(Geometry, "Geometry", IClassDesc::Flags::None))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Geometry::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        //_desc.registerPropertyHelperEx(Geometry, m_boundingBox.m_min, "AABBMin", IProperty::Flags::ReadOnly);
        //_desc.registerPropertyHelperEx(Geometry, m_boundingBox.m_max, "AABBMax", IProperty::Flags::ReadOnly);

        // TODO: registerMemberPropertyHelper ?
        _desc.registerProperty("Geometry", "m_min", (core::float3 *)(&((Geometry *)(nullptr))->m_aabb.m_min), "AABBMin", IProperty::Flags::ReadOnly);
        _desc.registerProperty("Geometry", "m_max", (core::float3 *)(&((Geometry *)(nullptr))->m_aabb.m_max), "AABBMax", IProperty::Flags::ReadOnly);

        return true;
    }
}
