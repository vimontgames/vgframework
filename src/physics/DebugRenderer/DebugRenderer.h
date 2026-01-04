#pragma once

#ifdef JPH_DEBUG_RENDERER

#pragma push_macro("new")
#undef new
#include "Jolt/Renderer/DebugRenderer.h"
#pragma pop_macro("new")  

#include "core/Types/Types.h"
#include "core/Singleton/Singleton.h"

namespace vg::core
{
    class IWorld;
}

namespace vg::renderer
{
    class IDebugGeometry;
}

namespace vg::physics
{
    class DebugRendererBatch final : public JPH::RefTargetVirtual
    {
    public:
        DebugRendererBatch();
        ~DebugRendererBatch();

        void AddRef() final override;
        void Release() final override;

        void setGeometry(renderer::IDebugGeometry * _geometry);
        renderer::IDebugGeometry * getGeometry() const;

    private:
        core::atomic<core::u32>     m_refCount = 0;
        renderer::IDebugGeometry *  m_debugGeometry = nullptr;
    };

    class DebugRenderer final : public JPH::DebugRenderer, public core::Singleton<DebugRenderer>
    {
    public:
        void                        init                ();
        void                        deinit              ();

        bool                        isInitialized       () const { return m_initDone; }

        void                        setWorld            (const core::IWorld * _world);
        const core::IWorld *        getWorld            () const;

        void						setCameraPos        (const core::float3 & _cameraPos);

        void                        DrawLine            (JPH::RVec3Arg _from, JPH::RVec3Arg _to, JPH::ColorArg _color) final override;
        void                        DrawTriangle        (JPH::RVec3Arg _v1, JPH::RVec3Arg _v2, JPH::RVec3Arg _v3, JPH::ColorArg _color, ECastShadow _castShadow) final override;
        void                        DrawText3D          (JPH::RVec3Arg _position, const JPH::string_view & _string, JPH::ColorArg _color, float _height) final override;

        JPH::DebugRenderer::Batch   CreateTriangleBatch (const JPH::DebugRenderer::Triangle * _triangles, int _triangleCount) final override;
        JPH::DebugRenderer::Batch   CreateTriangleBatch (const JPH::DebugRenderer::Vertex * _triangles, int _triangleCount, const JPH::uint32 * _indices, int _indexCount) override;

        void                        DrawGeometry        (JPH::RMat44Arg _modelMatrix, const JPH::AABox & _worldSpaceBounds, float _LODScaleSq, JPH::ColorArg _modelColor, const JPH::DebugRenderer::GeometryRef & _geometry, JPH::DebugRenderer::ECullMode _cullMode, JPH::DebugRenderer::ECastShadow _castShadow, JPH::DebugRenderer::EDrawMode _drawMode) override;

    private:
        bool                        m_initDone = false;
        const core::IWorld *        m_world = nullptr;

        // Last provided camera position
        core::float3				m_cameraPos;
        bool						m_cameraPosSet = false;
    };
}

#endif