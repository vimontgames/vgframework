#include "core/IGameObject.h"

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    VG_INLINE core::float4x4 UIComponent::getMatrix() const
    {
        core::float4x4 matrix = GetGameObject()->GetGlobalMatrix();

        if (m_useOffset)
            matrix[3].xyz += m_offset;

        return matrix;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::float4 UIComponent::getColor() const
    {
        return m_useColor ? m_color : core::float4(1,1,1,1);
    }
}