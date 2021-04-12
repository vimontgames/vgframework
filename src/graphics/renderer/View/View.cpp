#include "graphics/renderer/Precomp.h"
#include "View.h"
#include "core/Sector/Sector.h"

#if !VG_ENABLE_INLINE
#include "View.inl"
#endif

using namespace vg::core;

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    View::View(const CreateViewParams & _params) : 
        IView(_params)
    {
        m_viewInv = float4x4
        (
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, -1.0f, 0.0f,
            0.0f, 0.0f, -1.0f, 1.0f
        );
    }

    //--------------------------------------------------------------------------------------
    View::~View()
    {
        VG_SAFE_RELEASE(m_cameraSector);
    }

    //--------------------------------------------------------------------------------------
    void View::SetViewInvMatrix(const core::float4x4 & _viewInv)
    {
        m_viewInv = _viewInv;
    }

    //--------------------------------------------------------------------------------------
    const core::float4x4 & View::GetViewInvMatrix() const
    {
        return getViewInvMatrix();
    }

    //--------------------------------------------------------------------------------------
    void View::SetCameraSector(core::ISector * _cameraSector)
    {
        if ((core::Sector*)_cameraSector != m_cameraSector)
        {
            VG_SAFE_INCREASE_REFCOUNT(_cameraSector);
            VG_SAFE_RELEASE(m_cameraSector);
            m_cameraSector = (core::Sector*)_cameraSector;
        }
    }

    //--------------------------------------------------------------------------------------
    core::ISector * View::GetCameraSector() const
    {
        return getCameraSector();
    }
}