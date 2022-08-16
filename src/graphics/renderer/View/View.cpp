#include "graphics/renderer/Precomp.h"
#include "View.h"
#include "core/GameObject/GameObject.h"

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
    void View::SetupCamera(const core::float4x4 & _viewInv, core::float2 _nearFar, float _fovY)
    {
        m_viewInv = _viewInv;
        m_cameraNearFar = _nearFar;
        m_cameraFovY = _fovY;
    }

    //--------------------------------------------------------------------------------------
    const core::float4x4 & View::GetViewInvMatrix() const
    {
        return getViewInvMatrix();
    }

    //--------------------------------------------------------------------------------------
    core::float2 View::GetCameraNearFar() const
    {
        return getCameraNearFar();
    }

    //--------------------------------------------------------------------------------------
    float View::GetCameraFovY() const
    {
        return getCameraFovY();
    }

    //--------------------------------------------------------------------------------------
    void View::SetCameraSector(core::IGameObject * _cameraSector)
    {
        if ((core::IGameObject*)_cameraSector != m_cameraSector)
        {
            VG_SAFE_INCREASE_REFCOUNT(_cameraSector);
            VG_SAFE_RELEASE(m_cameraSector);
            m_cameraSector = (core::GameObject*)_cameraSector;
        }
    }

    //--------------------------------------------------------------------------------------
    core::IGameObject * View::GetCameraSector() const
    {
        return getCameraSector();
    }
}