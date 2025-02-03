#pragma once

#include "renderer/IDebugDraw.h"
#include "core/IWorld.h"
#include "core/Singleton/Singleton.h"
#include "core/Container/AtomicVector.h"
#include "gfx/RootSignature/RootSignature_consts.h"
#include "gfx/Shader/ShaderKey.h"

struct DebugDrawRootConstants3D;

namespace vg::core
{
    class AABB;
}

namespace vg::gfx
{
    class CommandList;
    class Buffer;
}

namespace vg::renderer
{
    class MeshGeometry;
    class View;

    vg_enum_class(DebugDrawFillMode, core::u8,
        Wireframe,
        Solid,
        Outline
    );

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

        core::IDebugDrawData *  CreateDebugDrawData() final override;

        void                    AddLine                     (const core::IWorld * _world, const core::float3 & _beginPos, const core::float3 & _endPos, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) final override;
        
        void                    AddWireframeCube            (const core::IWorld * _world, const core::float3 & _minPos, const core::float3 & _maxPos, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) final override;
        void                    AddWireframeSphere          (const core::IWorld * _world, float _radius, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) final override;
        void                    AddWireframeHemisphere      (const core::IWorld * _world, float _radius, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) final override;
        void                    AddWireframeCylinder        (const core::IWorld * _world, float _radius, float _height, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) final override;
        void                    AddWireframeTaperedCylinder (const core::IWorld * _world, float _topRadius, float _bottomRadius, float _height, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) final override;
        void                    AddWireframeCapsule         (const core::IWorld * _world, float _radius, float _height, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) final override;
        void                    AddWireframeTaperedCapsule  (const core::IWorld * _world, float _topRadius, float _bottomRadius, float _height, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) final override;
        void                    AddWireframeSquarePyramid   (const core::IWorld * _world, float _base, float _height, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) final override;

        void                    AddSolidCube                (const core::IWorld * _world, const core::float3 & _minPos, const core::float3 & _maxPos, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) final override;
        void                    AddSolidSphere              (const core::IWorld * _world, float _radius, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) final override;
        void                    AddSolidHemisphere          (const core::IWorld * _world, float _radius, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) final override;
        void                    AddSolidCylinder            (const core::IWorld * _world, float _radius, float _height, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) final override;
        void                    AddSolidSquarePyramid       (const core::IWorld * _world, float _base, float _height, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) final override;

        void                    drawCube                    (gfx::CommandList * _cmdList, DebugDrawFillMode _mode, bool _zTest, const core::float3 & _minPos, const core::float3 & _maxPos, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) const;
        void                    drawSphere                  (gfx::CommandList * _cmdList, DebugDrawFillMode _mode, bool _zTest, float _radius, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) const;
        void                    drawCylinder                (gfx::CommandList * _cmdList, DebugDrawFillMode _mode, bool _zTest, float _radius, float _height, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) const;
        void                    drawSquarePyramid           (gfx::CommandList * _cmdList, DebugDrawFillMode _mode, bool _zTest, float _base, float _height, core::u32 _color, const core::float4x4 & _matrix = core::float4x4::identity(), PickingID _pickingID = 0) const;

        void                    endFrame                    ();
        void                    reset                       ();

        core::u64               getDebugDrawCount           (const View * _view) const;

        void                    update                      (const View * _view, gfx::CommandList * _cmdList);
        void                    render                      (const View * _view, gfx::CommandList * _cmdList);

        void                    drawAABB                    (gfx::CommandList * _cmdList, const core::AABB & _aabb, const core::float4x4 & _world) const;
        void                    drawGrid                    (gfx::CommandList * _cmdList) const; 
        void                    drawAxis                    (gfx::CommandList * _cmdList) const;

    protected:
        void                    createBoxPrimitive          ();
        void                    createCubePrimitive         ();
        void                    createSquarePyramidPrimitive();
        void                    createGrid                  ();
        void                    createAxis                  ();
        void                    createIcoSpherePrimitive    (); 
        void                    createCylinderPrimitive     ();

        DrawData &              getDrawData                 (const View * _view);
        void                    clearDrawData               ();

        const WorldData *       getWorldData                (const core::IWorld * _world) const;
        WorldData *             getWorldData                (const core::IWorld * _world);

        void                    addLine                     (const core::IWorld * _world, const core::float3 & _beginPos, const core::float3 & _endPos, core::u32 _color, const core::float4x4 & _matrix);
        void                    addSquarePyramid            (const core::IWorld * _world, float _base, float _height, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID, DebugDrawFillMode _fillmode);
        void                    addSphere                   (const core::IWorld * _world, float _radius, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID, DebugDrawFillMode _fillmode);
        void                    addHemisphere               (const core::IWorld * _world, const float _radius, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID, DebugDrawFillMode _fillmode);
        void                    addCylinder                 (const core::IWorld * _world, float _radius, float _height, core::u32 _color, const core::float4x4 & _matrix, PickingID _pickingID, DebugDrawFillMode _fillmode);

        core::float4x4          getIcoSphereMatrix          (const core::float4x4 & _matrix, float _radius) const;
        core::float4x4          getCubeMatrix               (const core::float4x4 & _matrix, const core::float3 & _minPos, const core::float3 & _maxPos) const;
        core::float4x4          getCylinderMatrix           (const core::float4x4 & _matrix, float _radius, float _height) const;
        core::float4x4          getSquarePyramidMatrix      (const core::float4x4 & _matrix, float _base, float _height) const;

        void                    setupZFail                  (gfx::CommandList * _cmdList) const;
        void                    setupZPass                  (gfx::CommandList * _cmdList) const;
        void                    setupOutline                (gfx::CommandList * _cmdList) const;

        struct DebugDrawInstanceData
        {
            core::float4x4  world;
            PickingID       pickingID;
            core::u32       color;

            float getTaper() const { return 1.0f; }
        };

        struct DebugDrawInstanceDataCylinder : public DebugDrawInstanceData
        {
            float           taper;      // Top/Bottom ratio

            float getTaper() const { return taper; }
        };

        bool                                    setupDebugModelInstances    (gfx::CommandList * _cmdList, const MeshGeometry * _geometry, DebugDrawFillMode _fillmode, DebugDrawRootConstants3D & _debugDraw3D, core::uint & _indexCount) const;

        void                                    drawDebugModelInstance      (gfx::CommandList * _cmdList, DebugDrawFillMode _mode, bool _zTest, const MeshGeometry * _geometry, const core::float4x4 & _matrix, core::u32 _color, float _taper, PickingID _pickingID) const;
        template <typename T, size_t N> void    drawDebugModelInstances     (gfx::CommandList * _cmdList, const MeshGeometry * _geometry, const core::vector<T>(&_instances)[N], DebugDrawFillMode _fillmode) const;
       
    private:
        gfx::RootSignatureHandle        m_debugDrawSignatureHandle;
        gfx::ShaderKey                  m_debugDrawShaderKey;
        gfx::ShaderKey                  m_debugDrawOutlineShaderKey;

        MeshGeometry *                  m_box = nullptr;
        gfx::Buffer *                   m_gridVB = nullptr;
        gfx::Buffer *                   m_axisVB = nullptr;
        MeshGeometry *                  m_cube = nullptr;
        MeshGeometry *                  m_squarePyramid = nullptr;
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

        using DebugDrawCubeData = DebugDrawInstanceData;
        using DebugDrawSquarePyramidData = DebugDrawInstanceData;
        using DebugDrawIcoSphereData = DebugDrawInstanceData;
        using DebugDrawHemiSphereData = DebugDrawInstanceData;
        using DebugDrawCylinderData = DebugDrawInstanceDataCylinder;

        struct WorldData : public core::IDebugDrawData
        {
            WorldData();
            ~WorldData();

            core::atomicvector<DebugDrawLineData>       m_lines;
            core::vector<DebugDrawIcoSphereData>        m_squarePyramid[core::enumCount<DebugDrawFillMode>()]; 
            core::vector<DebugDrawSquarePyramidData>    m_cubes[core::enumCount<DebugDrawFillMode>()];
            core::vector<DebugDrawIcoSphereData>        m_icoSpheres[core::enumCount<DebugDrawFillMode>()];
            core::vector<DebugDrawHemiSphereData>       m_hemiSpheres[core::enumCount<DebugDrawFillMode>()];
            core::vector<DebugDrawCylinderData>         m_cylinders[core::enumCount<DebugDrawFillMode>()];
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