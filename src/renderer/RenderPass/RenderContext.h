#pragma once

namespace vg::renderer
{
    enum class ShaderPass : core::u8;

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
        ShaderPass m_shaderPass;
    };
}