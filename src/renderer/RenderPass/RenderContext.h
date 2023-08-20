#pragma once

namespace vg::renderer
{
    class RenderContext
    {
    public:
        RenderContext()
        {
            m_viewProj.identity();
        }

        core::float4x4 m_viewProj;
        bool m_toolmode = false;
        bool m_wireframe = false;
    };
}