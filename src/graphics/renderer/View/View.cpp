#include "graphics/renderer/Precomp.h"
#include "View.h"
#include "core/GameObject/GameObject.h"
#include "graphics/driver/ITexture.h"
#include "graphics/renderer/Job/Culling/ViewCullingJob.h"

#if !VG_ENABLE_INLINE
#include "View.inl"
#endif

// Blob
#include "Forward/ForwardView.hpp"

using namespace vg::core;
using namespace vg::graphics::driver;

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    View::View(const CreateViewParams & _params) : 
        IView(_params)
    {
        m_viewID.type = _params.type;

        m_size = _params.size;
        m_offset = _params.offset;

        m_viewInv = float4x4
        (
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, -1.0f, 0.0f,
            0.0f, 0.0f, -1.0f, 1.0f
        );

        SetUniverse(_params.universe);

        if (_params.target)
        {
            VG_SAFE_INCREASE_REFCOUNT(_params.target);
            m_renderTarget = (Texture*)_params.target;
        }

        m_cullingJob = new ViewCullingJob("ViewCulling", this, &m_cullingJobResult);
    }

    //--------------------------------------------------------------------------------------
    View::~View()
    {
        VG_SAFE_RELEASE(m_renderTarget);
        VG_SAFE_RELEASE(m_cullingJob);
    }

    //--------------------------------------------------------------------------------------
    void View::SetRenderTarget(ITexture * _renderTarget)
    {
        setRenderTarget((Texture*)_renderTarget);
    }

    //--------------------------------------------------------------------------------------
    ITexture * View::GetRenderTarget() const
    {
        return (ITexture*)getRenderTarget();
    }

    //--------------------------------------------------------------------------------------
    // Setup Right-Handed perspective projection matrix:
    // 
    // sx = sy/_ar
    // sy = 1.0f/tan(fov*0.5f)  
    // q = far / (near - far);
    //
    // sx   0    0     0
    //  0  sy    0     0
    //  0   0    q    -1
    //  0   0   zn*q   0
    //--------------------------------------------------------------------------------------
    float4x4 View::setPerspectiveProjectionRH(float _fov, float _ar, float _near, float _far)
    {
        const float sy = 1.0f / tan(_fov*0.5f);
        const float sx = sy / _ar;
        const float q = _far / (_near - _far);
        const float nq = _near * q;

        float4x4 mProj
        (
            sx, 0, 0,  0,
            0, sy, 0,  0,
            0, 0,  q, -1,
            0, 0, nq,  0
        );

        return mProj;
    }

    //--------------------------------------------------------------------------------------
    void View::SetupCamera(const core::float4x4 & _viewInv, core::float2 _nearFar, float _fovY)
    {
        m_viewInv = _viewInv;
        m_cameraNearFar = _nearFar;
        m_cameraFovY = _fovY;

        const auto size = getSize();
        const float fovY = getCameraFovY();
        const float2 nearFar = getCameraNearFar();
        const float ar = float(size.x) / float(size.y);

        float4x4 proj = setPerspectiveProjectionRH(fovY, ar, nearFar.x, nearFar.y);
        m_viewProj = mul(getViewInvMatrix(), proj);
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
    void View::SetUniverse(IUniverse* _universe)
    {
        if (_universe != m_cameraUniverse)
            m_cameraUniverse = _universe;
    }

    //--------------------------------------------------------------------------------------
    IUniverse* View::GetUniverse() const
    {
        return getUniverse();
    }

    //--------------------------------------------------------------------------------------
    u32 View::release()
    {
        return IView::release();
    }

    //--------------------------------------------------------------------------------------
    void View::SetSize(core::uint2 _size)
    {
        setSize(_size);
    }

    //--------------------------------------------------------------------------------------
    core::uint2 View::GetSize() const
    {
        return getSize();
    }

    //--------------------------------------------------------------------------------------
    void View::SetOffset(core::int2 _offset)
    {
        setOffset(_offset);
    }

    //--------------------------------------------------------------------------------------
    core::int2 View::GetOffset() const
    {
        return getOffset();
    }

    //--------------------------------------------------------------------------------------
    void View::SetViewID(ViewID _viewID)
    {
        setViewID(_viewID);
    }

    //--------------------------------------------------------------------------------------
    ViewID View::GetViewID() const
    {
        return getViewID();
    }

    //--------------------------------------------------------------------------------------
    void View::SetActive(bool _active)
    {
        m_active = _active;
    }
    
    //--------------------------------------------------------------------------------------
    bool View::IsActive() const
    {
        return m_active;
    }
}