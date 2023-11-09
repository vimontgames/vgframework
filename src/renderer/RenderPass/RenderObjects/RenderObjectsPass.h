#pragma once

#include "renderer/RenderPass/RenderPass.h"
#include "renderer/RenderPass/RenderContext.h"

namespace vg::renderer
{
    class RenderContext;
    class GraphicInstanceList;
    class MeshGeometry;
    class AABB;

    //--------------------------------------------------------------------------------------
    class RenderObjectsPass : public RenderPass
    {
    public:
        RenderObjectsPass(const core::string & _name);
        ~RenderObjectsPass();

    protected:
        virtual void    DrawGraphicInstances    (const RenderContext & _renderContext, gfx::CommandList * _cmdList, const GraphicInstanceList & _graphicInstancesList) const;

        void            drawAABB                (gfx::CommandList * _cmdList, const AABB & _aabb, const core::float4x4 & _world) const;
        void            drawGrid                (gfx::CommandList * _cmdList) const;
        void            drawAxis                (gfx::CommandList * _cmdList) const;

    private:
        void            createUnitBox           ();
        void            createGrid              ();
        void            createAxis              ();

        void            destroyUnitBox          ();
        void            destroyGrid             ();
        void            destroyAxis             ();
        
    private:
        gfx::RootSignatureHandle    m_debugDrawSignatureHandle;
        gfx::ShaderKey              m_debugDrawShaderKey;
        MeshGeometry *              m_unitBox = nullptr;
        gfx::Buffer *               m_gridVB = nullptr;
        gfx::Buffer *               m_axisVB = nullptr;
    };
}