#pragma once

namespace vg::renderer
{
    enum class ShaderPass : core::u8;

    class RenderContext
    {
    public:
        RenderContext()
        {
            m_view.identity();
            m_proj.identity();
        }

        core::float4x4 m_view;
        core::float4x4 m_proj;
        bool m_toolmode = false;
        bool m_raytracing = false;
        bool m_wireframe = false;
        bool m_alphatest = false;
        ShaderPass m_shaderPass;
    };
}