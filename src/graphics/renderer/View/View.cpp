#include "graphics/renderer/Precomp.h"
#include "View.h"

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

    }

    //--------------------------------------------------------------------------------------
    void View::setViewInvMatrix(const core::float4x4 & _viewInv)
    {
        m_viewInv = _viewInv;
    }

    //--------------------------------------------------------------------------------------
    const core::float4x4 & View::getViewInvMatrix() const
    {
        return m_viewInv;
    }
}