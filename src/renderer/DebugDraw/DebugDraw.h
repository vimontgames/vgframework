#pragma once

#include "renderer/IDebugDraw.h"
#include "core/IWorld.h"
#include "core/Singleton/Singleton.h"
#include "core/Container/AtomicVector.h"
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
    class View;

    //--------------------------------------------------------------------------------------
    // Primitives to draw using 'AddLine', 'AddWireframe' are added to a common list but each
    // visible view will build its own buffer so that culling can possibly be done
    //--------------------------------------------------------------------------------------
    class DebugDraw final : public IDebugDraw, public core::Singleton<DebugDraw>
    {
        struct DrawData;
        struct WorldData;

    public:
        DebugDraw();
        ~DebugDraw();

        void        AddLine                 (const core::IWorld * _world, const core::float3 & _beginPos, const core::float3 & _endPos, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity()) final override;
        void        AddWireframeBox         (const core::IWorld * _world, const core::float3 & _minPos, const core::float3 & _maxPos, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity()) final override;
        void        AddWireframeSphere      (const core::IWorld * _world, float _radius, core::u32 _color, const core::float4x4 _matrix = core::float4x4::identity()) final override;
        void        AddHemisphere           (const core::IWorld * _world, float _radius, core::u32 _color, const core::float4x4 _matrix = core::float4x4::identity()) final override;
        void        AddCylinder             (const core::IWorld * _world, float _radius, float _height, core::u32 _color, const core::float4x4 _matrix = core::float4x4::identity()) final override;
        void        AddCapsule              (const core::IWorld * _world, float _radius, float _height, core::u32 _color, const core::float4x4 _matrix = core::float4x4::identity()) final override;

        void        endFrame                ();
        void        reset                   ();

        void        update                  (const View * _view, gfx::CommandList * _cmdList);
        void        render                  (const View * _view, gfx::CommandList * _cmdList);

        void        drawAABB                (gfx::CommandList * _cmdList, const AABB & _aabb, const core::float4x4 & _world) const;
        void        drawGrid                (gfx::CommandList * _cmdList) const; 
        void        drawAxis                (gfx::CommandList * _cmdList) const;

    protected:
        void        createBoxPrimitive      ();
        void        createGrid              ();
        void        createAxis              ();
        void        createIcoSpherePrimitive(); 
        void        createCylinderPrimitive ();

        DrawData &  getDrawData             (const View * _view);
        void        clearDrawData           ();

        WorldData * getWorldData            (const core::IWorld * _world);
        void        clearWorldData          ();

        void        addLine                 (const core::IWorld * _world, const core::float3 & _beginPos, const core::float3 & _endPos, core::u32 _color, const core::float4x4 & _matrix);
         
        struct DebugDrawInstanceData
        {
            core::float4x4 world;
            core::u32 color;
        };

        void        drawDebugModelInstances (gfx::CommandList * _cmdList, const MeshGeometry * _geometry, const core::vector<DebugDrawInstanceData> & _instances);

    private:
        gfx::RootSignatureHandle        m_debugDrawSignatureHandle;
        gfx::ShaderKey                  m_debugDrawShaderKey;

        MeshGeometry *                  m_box = nullptr;
        gfx::Buffer *                   m_gridVB = nullptr;
        gfx::Buffer *                   m_axisVB = nullptr;
        MeshGeometry *                  m_icoSphere = nullptr;
        MeshGeometry *                  m_hemiSphere = nullptr;
        MeshGeometry *                  m_cylinder = nullptr;

        struct DebugDrawLineData
        {
            core::float4x4 world;
            core::float3 beginPos;
            core::u32 beginColor;
            core::float3 endPos;
            core::u32 endColor;
        };

        using DebugDrawIcoSphereData = DebugDrawInstanceData;
        using DebugDrawHemiSphereData = DebugDrawInstanceData;
        using DebugDrawCylinderData = DebugDrawInstanceData;

        struct WorldData : public core::IDebugDrawData
        {
            WorldData();
            ~WorldData();

            core::atomicvector<DebugDrawLineData>   m_lines;
            core::vector<DebugDrawIcoSphereData>    m_icoSpheres;
            core::vector<DebugDrawHemiSphereData>   m_hemiSpheres;
            core::vector<DebugDrawCylinderData>     m_cylinders;
        };

        struct DrawData
        {
            gfx::Buffer *   m_debugDrawVB = nullptr;
            core::uint      m_debugDrawVBSize;

            core::u32       m_linesVBOffset;
            core::u32       m_linesToDraw = 0;
        };

        core::unordered_map<const View *, DrawData> m_drawData;
    };
}