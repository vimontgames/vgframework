#include "graphics/driver/Precomp.h"
#include "View.h"
#include "core/GameObject/GameObject.h"
#include "graphics/driver/ITexture.h"

#if !VG_ENABLE_INLINE
#include "View.inl"
#endif

using namespace vg::core;

namespace vg::graphics::driver
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
    }

    //--------------------------------------------------------------------------------------
    void View::SetRenderTarget(driver::ITexture * _renderTarget)
    {
        setRenderTarget((Texture*)_renderTarget);
    }

    //--------------------------------------------------------------------------------------
    driver::ITexture * View::GetRenderTarget() const
    {
        return (ITexture*)getRenderTarget();
    }

    //--------------------------------------------------------------------------------------
    View::~View()
    {
        VG_SAFE_RELEASE(m_renderTarget);
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
    void View::SetUniverse(core::IUniverse* _universe)
    {
        if (_universe != m_cameraUniverse)
            m_cameraUniverse = _universe;
    }

    //--------------------------------------------------------------------------------------
    core::IUniverse* View::GetUniverse() const
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
}