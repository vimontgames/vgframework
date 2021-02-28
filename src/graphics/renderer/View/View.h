#pragma once

#include "graphics/renderer/IView.h"

namespace vg::graphics::renderer
{
    class View : public IView
    {
    public:
        View(const CreateViewParams & _params);
        ~View();

        void setViewInvMatrix(const core::float4x4 & _viewInv) override;
        const core::float4x4 & getViewInvMatrix() const override;

    private:
        core::float4x4  m_viewInv = core::float4x4::identity();
    };
}