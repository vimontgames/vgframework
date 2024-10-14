#include "core/IGameObject.h"

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    VG_INLINE core::uint2 UIComponent::getSize() const
    {
        return m_size;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void UIComponent::setSize(core::uint2 _size)
    {
        m_size = _size;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::float3 UIComponent::getOffset() const
    {
        return m_useOffset ? m_offset : core::float3(0, 0, 0);
           
    }
    
    //--------------------------------------------------------------------------------------
    VG_INLINE core::float4x4 UIComponent::getMatrix() const
    {
        return GetGameObject()->GetGlobalMatrix();
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::float4 UIComponent::getColor() const
    {
        return m_useColor ? m_color : core::float4(1,1,1,1);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE renderer::IUIManager * UIComponent::getUIManager() const
    {
        return m_uiManager;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::float2 UIComponent::getCenter() const
    {
        return m_useCenter ? m_center : core::float2(0.5f, 0.5f);
    }
}