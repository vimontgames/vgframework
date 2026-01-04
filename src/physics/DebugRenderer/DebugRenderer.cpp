#include "physics/Precomp.h"

#ifdef JPH_DEBUG_RENDERER
#include "DebugRenderer.h"
#include "physics/Physics.h"
#include "renderer/IDebugDraw.h"

using namespace vg::core;
using namespace vg::renderer;

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    DebugRendererBatch::DebugRendererBatch()
    {

    }

    //--------------------------------------------------------------------------------------
    DebugRendererBatch::~DebugRendererBatch()
    {
        VG_SAFE_RELEASE(m_debugGeometry);
    }

    //--------------------------------------------------------------------------------------
    void DebugRendererBatch::setGeometry(renderer::IDebugGeometry * _geometry)
    {
        m_debugGeometry = _geometry;
    }

    //--------------------------------------------------------------------------------------
    renderer::IDebugGeometry * DebugRendererBatch::getGeometry() const
    {
        return m_debugGeometry;
    }

    //--------------------------------------------------------------------------------------
    void DebugRendererBatch::AddRef() 
    { 
        ++m_refCount;
    }

    //--------------------------------------------------------------------------------------
    void DebugRendererBatch::Release() 
    {
        if (--m_refCount == 0) 
            delete this;
    }

    //--------------------------------------------------------------------------------------
    void DebugRenderer::init()
    {
        VG_ASSERT(!m_initDone);
        Initialize();
        m_initDone = true;
    }

    //--------------------------------------------------------------------------------------
    void DebugRenderer::deinit()
    {
        if (m_initDone)
        {
            m_initDone = false;
        }
    }
    
    //--------------------------------------------------------------------------------------
    void DebugRenderer::setWorld(const core::IWorld * _world) 
    { 
        m_world = _world; 
    }

    //--------------------------------------------------------------------------------------
    const core::IWorld * DebugRenderer::getWorld() const
    {
        return m_world;
    }

    //--------------------------------------------------------------------------------------
    void DebugRenderer::setCameraPos(const core::float3 & _cameraPos)
    {
        m_cameraPos = _cameraPos;
        m_cameraPosSet = true;
    }

    //--------------------------------------------------------------------------------------
    void DebugRenderer::DrawLine(JPH::RVec3Arg _from, JPH::RVec3Arg _to, JPH::ColorArg _color)
    {
        IDebugDraw * debugDraw = Physics::get()->getDebugDraw();
        debugDraw->AddLine(getWorld(), fromJoltVec3(_from), fromJoltVec3(_to), _color.GetUInt32());
    }

    //--------------------------------------------------------------------------------------
    void DebugRenderer::DrawTriangle(JPH::RVec3Arg _v1, JPH::RVec3Arg _v2, JPH::RVec3Arg _v3, JPH::ColorArg _color, ECastShadow _castShadow)
    {

    }

    //--------------------------------------------------------------------------------------
    void DebugRenderer::DrawText3D(JPH::RVec3Arg _position, const JPH::string_view & _string, JPH::ColorArg _color, float _height)
    {

    }

    //--------------------------------------------------------------------------------------
    JPH::DebugRenderer::Batch DebugRenderer::CreateTriangleBatch(const JPH::DebugRenderer::Triangle * _triangles, int _triangleCount)
    {
        DebugRendererBatch * batch = new DebugRendererBatch();
        if (_triangles == nullptr || _triangleCount == 0)
            return batch;

        IDebugDraw * debugDraw = Physics::get()->getDebugDraw();

        DebugVertex * debugVertices = new DebugVertex[_triangleCount*3];

        for (int i = 0; i < _triangleCount; ++i)
        {
            const JPH::DebugRenderer::Triangle & triangle = _triangles[i];

            for (int v = 0; v < 3; ++v)
            {
                const JPH::DebugRenderer::Vertex & vert = triangle.mV[v];
                DebugVertex & vtx = debugVertices[i * 3 + v];
                vtx.position = fromJoltFloat3(vert.mPosition);
                vtx.normal = fromJoltFloat3(vert.mNormal);
                vtx.uv = fromJoltFloat2(vert.mUV);
                vtx.color = vert.mColor.GetUInt32();
            }
        }

        renderer::IDebugGeometry * debugGeometry = debugDraw->CreateGeometry(debugVertices, _triangleCount*3);
        batch->setGeometry(debugGeometry);

        delete[] debugVertices;

        return batch;
    }

    //--------------------------------------------------------------------------------------
    JPH::DebugRenderer::Batch DebugRenderer::CreateTriangleBatch(const JPH::DebugRenderer::Vertex * _vertices, int _vertexCount, const JPH::uint32 * _indices, int _indexCount)
    {
        DebugRendererBatch * batch = new DebugRendererBatch();
        if (_vertices == nullptr || _vertexCount == 0)
            return batch;

        IDebugDraw * debugDraw = Physics::get()->getDebugDraw();

        DebugVertex * debugVertices = new DebugVertex[_vertexCount];

        for (int i = 0; i < _vertexCount; ++i)
        {
            const JPH::DebugRenderer::Vertex & vert = _vertices[i];
            DebugVertex & v = debugVertices[i];

            v.position = fromJoltFloat3(vert.mPosition);
            v.normal = fromJoltFloat3(vert.mNormal);
            v.uv = fromJoltFloat2(vert.mUV);
            v.color = vert.mColor.GetUInt32();
        }

        renderer::IDebugGeometry * debugGeometry = debugDraw->CreateIndexedGeometry(debugVertices, _vertexCount, _indices, _indexCount);
        batch->setGeometry(debugGeometry);

        delete[] debugVertices;

        return batch;
    }

    //--------------------------------------------------------------------------------------
    void DebugRenderer::DrawGeometry(JPH::RMat44Arg _modelMatrix, const JPH::AABox & _worldSpaceBounds, float _LODScaleSq, JPH::ColorArg _modelColor, const JPH::DebugRenderer::GeometryRef & _geometry, JPH::DebugRenderer::ECullMode _cullMode, JPH::DebugRenderer::ECastShadow _castShadow, JPH::DebugRenderer::EDrawMode _drawMode)
    {
        IDebugDraw * debugDraw = Physics::get()->getDebugDraw();

        const LOD * lod = _geometry->mLODs.data();
        //if (mCameraPosSet)
        //    lod = &_geometry->GetLOD(Vec3(mCameraPos), _worldSpaceBounds, _LODScaleSq);

        DebugRendererBatch * batch = (DebugRendererBatch*) lod->mTriangleBatch.GetPtr();
        IDebugGeometry * model = batch->getGeometry();

        float4x4 matrix = fromJoltMatrix(_modelMatrix);

        switch (_drawMode)
        {
            default:
                VG_ASSERT(false);

            case EDrawMode::Wireframe:
                debugDraw->AddWireframeGeometry(getWorld(), model, _modelColor.GetUInt32(), matrix);
                break;

            case EDrawMode::Solid:
                debugDraw->AddSolidGeometry(getWorld(), model, _modelColor.GetUInt32(), matrix);
                break;
        }
    }
}
#endif // JPH_DEBUG_RENDERER