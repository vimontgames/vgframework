#pragma once

#include "graphics/driver/FrameGraph/UserPass.h"

namespace vg::graphics::driver
{
    class Buffer;
}

namespace vg::graphics::renderer
{
    class MeshModel;
    class AABB;

    //--------------------------------------------------------------------------------------
    class ForwardPass : public driver::UserPass
    {
    public:
        const char * getClassName() const final { return "ForwardPass"; }

        ForwardPass();
        ~ForwardPass();

        void setup(const driver::RenderContext & _renderContext, double _dt) override;
        void draw(const driver::RenderContext & _renderContext, driver::CommandList * _cmdList) const override;

    protected:
        void createGrid();
        void destroyGrid();
        void drawGrid(driver::CommandList * _cmdList, const core::float4x4 & _viewProj) const;

        void createAxis();
        void destroyAxis();
        void drawAxis(driver::CommandList * _cmdList, const core::float4x4 & _viewProj) const;

        void createUnitBox();
        void destroyUnitBox();
        void drawAABB(driver::CommandList * _cmdList, const AABB & _aabb, const core::float4x4 & _world, const core::float4x4 & _viewProj) const;

    private:
        driver::RootSignatureHandle     m_rootSignatureHandle;
        driver::ShaderKey               m_forwardShaderKey;
        driver::ShaderKey               m_wireframeShaderKey;

        driver::Buffer *                m_gridVB = nullptr;
        driver::Buffer *                m_axisVB = nullptr;

        driver::Buffer *                m_unitBoxIB = nullptr;
        driver::Buffer *                m_unitBoxVB = nullptr;
    };
}