#pragma once

#include "graphics/driver/FrameGraph/UserPass.h"

namespace vg::graphics::driver
{
    class Buffer;
}

namespace vg::graphics::renderer
{
    class MeshModel;

    //--------------------------------------------------------------------------------------
    class TestPass3D : public driver::UserPass
    {
    public:
        const char * getClassName() const final { return "TestPass3D"; }

        TestPass3D();
        ~TestPass3D();

        void setup(double _dt) override;
        void draw(driver::CommandList * _cmdList) const override;

    protected:
        void createGrid();
        void destroyGrid();
        void drawGrid(driver::CommandList * _cmdList, const core::float4x4 & _viewProj) const;

        void createAxis();
        void destroyAxis();
        void drawAxis(driver::CommandList * _cmdList, const core::float4x4 & _viewProj) const;

    private:
        driver::RootSignatureHandle     m_rootSignatureHandle;
        driver::ShaderKey               m_forwardShaderKey;
        driver::ShaderKey               m_wireframeShaderKey;

        driver::Buffer *                m_gridVB = nullptr;
        driver::Buffer *                m_axisVB = nullptr;
    };
}