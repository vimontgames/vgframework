#include "renderer/Precomp.h"
#include "core/Object/AutoRegisterClass.h"
#include "Geometry.h"

#include "gfx/Resource/Buffer.h"

#if !VG_ENABLE_INLINE
#include "Geometry.inl"
#endif

#include "Batch/Batch.hpp"
#include "Mesh/MeshGeometry.hpp"

namespace vg::renderer
{
    VG_REGISTER_OBJECT_CLASS(Geometry, "Geometry");

    //--------------------------------------------------------------------------------------
    bool Geometry::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEx(Geometry, m_aabb.m_min, "AABBMin", IProperty::Flags::ReadOnly);
        setPropertyDescription(Geometry, m_aabb.m_min, "Axis-Aligned Bounding Box minimum bounds");

        registerPropertyEx(Geometry, m_aabb.m_max, "AABBMax", IProperty::Flags::ReadOnly);
        setPropertyDescription(Geometry, m_aabb.m_max, "Axis-Aligned Bounding Box maximum bounds");

        return true;
    }
}
