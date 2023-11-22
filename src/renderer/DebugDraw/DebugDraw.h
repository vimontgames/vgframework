#pragma once

#include "core/Singleton/Singleton.h"
#include "renderer/IDebugDraw.h"
#include "gfx/RootSignature/RootSignature_consts.h"
#include "gfx/Shader/ShaderKey.h"

namespace vg::gfx
{
    class CommandList;
    class Buffer;
}

namespace vg::renderer
{
    class MeshGeometry;
    class AABB;

    class DebugDraw : public IDebugDraw, public core::Singleton<DebugDraw>
    {
    public:
        DebugDraw();
        ~DebugDraw();

        void    AddLine                 (const core::float3 & _beginPos, const core::float3 & _endPos, core::u32 _color, const core::float4x4 & _world = core::float4x4::identity()) final override;
        void    AddWireframeBox         (const core::float3 & _minPos, const core::float3 & _maxPos, core::u32 _color, const core::float4x4 & _world = core::float4x4::identity()) final override;

        void    update                  (gfx::CommandList * _cmdList);
        void    render                  (gfx::CommandList * _cmdList);

        void    drawAABB                (gfx::CommandList * _cmdList, const AABB & _aabb, const core::float4x4 & _world) const;
        void    drawGrid                (gfx::CommandList * _cmdList) const; 
        void    drawAxis                (gfx::CommandList * _cmdList) const;

    protected:
        void    createDebugDrawBuffer   ();
        void    createBoxPrimitive      ();
        void    createGrid              ();
        void    createAxis              ();

    private:

        gfx::Buffer *                   m_debugDrawVB = nullptr;
        core::uint                      m_debugDrawVBSize;

        struct DebugDrawLineData
        {
            core::float4x4 world;
            core::float3 beginPos;
            core::u32 beginColor;
            core::float3 endPos;
            core::u32 endColor;
        };
        core::vector<DebugDrawLineData> m_lines;
        core::u32                       m_linesVBOffset;
        core::u32                       m_linesToDraw = 0;

        struct DebugDrawBoxData
        {
            core::float4x4 world;
            core::float3 minPos;
            core::float3 maxPos;
            core::u32 color;
        };
        core::vector<DebugDrawBoxData>  m_wireframeBoxes;
        core::u32                       m_wireframeBoxesOffset;
        core::u32                       m_wireframeBoxesCount = 0;

        gfx::RootSignatureHandle        m_debugDrawSignatureHandle;
        gfx::ShaderKey                  m_debugDrawShaderKey;
        MeshGeometry *                  m_box = nullptr;
        gfx::Buffer *                   m_gridVB = nullptr;
        gfx::Buffer *                   m_axisVB = nullptr;
    };
}