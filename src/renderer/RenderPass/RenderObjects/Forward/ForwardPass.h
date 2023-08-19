#pragma once

#include "renderer/RenderPass/RenderObjects/RenderObjectsPass.h"

namespace vg::gfx
{
    class Buffer;
    class DynamicBuffer;
}

namespace vg::renderer
{
    class MeshModel;
    class AABB;

    //--------------------------------------------------------------------------------------
    class ForwardPass : public RenderObjectsPass
    {
    public:
        const char * getClassName() const final { return "ForwardPass"; }

        ForwardPass();
        ~ForwardPass();

        void setup(const gfx::RenderContext & _renderContext, double _dt) override;
        void draw(const gfx::RenderContext & _renderContext, gfx::CommandList * _cmdList) const override;

    protected:
        void createGrid();
        void destroyGrid();
        void drawGrid(gfx::CommandList * _cmdList, const core::float4x4 & _viewProj) const;

        void createAxis();
        void destroyAxis();
        void drawAxis(gfx::CommandList * _cmdList, const core::float4x4 & _viewProj) const;

        void createUnitBox();
        void destroyUnitBox();
        void drawAABB(gfx::CommandList * _cmdList, const AABB & _aabb, const core::float4x4 & _world, const core::float4x4 & _viewProj) const;

    private:
        gfx::RootSignatureHandle    m_rootSignatureHandle;
        gfx::ShaderKey              m_forwardShaderKey;
        gfx::ShaderKey              m_drawGridShaderKey;
        gfx::ShaderKey              m_wireframeShaderKey;

        gfx::Buffer *               m_gridVB = nullptr;
        gfx::Buffer *               m_axisVB = nullptr;

        gfx::Buffer *               m_unitBoxIB = nullptr;
        gfx::Buffer *               m_unitBoxVB = nullptr;

        //gfx::DynamicBuffer *        m_dynamicBuffer = nullptr;
        //gfx::Buffer *               m_updatedBuffer = nullptr;
    };
}