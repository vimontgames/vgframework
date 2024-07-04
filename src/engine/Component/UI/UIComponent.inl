#include "core/IGameObject.h"

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    VG_INLINE core::float3 UIComponent::getOffset() const
    {
        return m_useOffset ? m_offset : core::float3(0, 0, 0);
           
    }
    
    //--------------------------------------------------------------------------------------
    VG_INLINE core::float4x4 UIComponent::getMatrix() const
    {
        core::float4x4 matrix = GetGameObject()->GetGlobalMatrix();

        matrix[3].xyz += getOffset();

        return matrix;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::float4 UIComponent::getColor() const
    {
        return m_useColor ? m_color : core::float4(1,1,1,1);
    }
}