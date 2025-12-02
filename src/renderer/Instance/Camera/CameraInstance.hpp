#include "CameraInstance.h"

#if !VG_ENABLE_INLINE
#include "CameraInstance.inl"
#endif

using namespace vg::core;
using namespace vg::gfx;

namespace vg::renderer
{
    VG_REGISTER_OBJECT_CLASS_EX(CameraInstance, "Camera Instance", core::ClassDescFlags::Instance);

    //--------------------------------------------------------------------------------------
    CameraInstance::CameraInstance(const core::string & _name, core::IObject * _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    CameraInstance::~CameraInstance()
    {

    }

    //--------------------------------------------------------------------------------------
    bool CameraInstance::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool CameraInstance::Cull(const CullingOptions & _cullingOptions, CullingResult * _cullingResult)
    {
        _cullingResult->m_output->add((const CameraInstance*)this);

        if (asBool(ObjectRuntimeFlags::Selected & getObjectRuntimeFlags()))
            _cullingResult->m_output->add(GraphicInstanceListType::Outline, this);

        return true;
    }

    //--------------------------------------------------------------------------------------
    void CameraInstance::Draw(const RenderContext & _renderContext, gfx::CommandList * _cmdList) const
    {
        const auto * view = static_cast<const View *>(_renderContext.m_renderPass->getView());
        auto * debugDraw = DebugDraw::get();

        switch (_renderContext.m_shaderPass)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_renderContext.m_shaderPass);
                break;

            case ShaderPass::ZOnly:
                break;

            case ShaderPass::Forward:
            case ShaderPass::Deferred:
            case ShaderPass::Outline:
            {
                const u32 color = packRGBA8(GetColor());
                const PickingID pickingID = GetPickingID();
                //const auto & frustum = view->getCameraFrustum();
                //frustum.draw(_renderContext.m_renderPass->getWorld(), color);
               
                const float4x4 matrix = GetGlobalMatrix(); // get view matrix?

                float4x4 mCube = float4x4::identity();
                mCube[0].xyz = float3(0.5f, 0.0f, 0.0f);
                mCube[1].xyz = float3(0.0f, 0.5f, 0.0f);
                mCube[2].xyz = float3(0.0f, 0.0f, 1.0f);
                mCube[3].xyz = float3(0.0f, 0.0f, 1.0f);
                mCube = mul(mCube, matrix);

                float4x4 mSquarePyramid = float4x4::identity();
                mSquarePyramid[0].xyz = float3(0.5f, 0.0f, 0.0f);
                mSquarePyramid[1].xyz = float3(0.0f, 0.5f, 0.0f);
                mSquarePyramid[2].xyz = float3(0.0f, 0.0f, 0.5f);
                mSquarePyramid[3].xyz = float3(0.0f, 0.0f, 0.25f);
                mSquarePyramid = mul(mSquarePyramid, matrix);

                if (ShaderPass::Outline == _renderContext.m_shaderPass)
                {
                    debugDraw->drawCube(_cmdList, DebugDrawFillMode::Outline, true, (float3)-0.25f, (float3)0.25f, color, mCube, pickingID);
                    debugDraw->drawSquarePyramid(_cmdList, DebugDrawFillMode::Outline, true, 1.0f, 1.0f, color, mSquarePyramid, pickingID);
                }
                else
                {
                    debugDraw->drawCube(_cmdList, DebugDrawFillMode::Solid, false, (float3)-0.25f, (float3)0.25f, color, mCube, pickingID);
                    debugDraw->drawSquarePyramid(_cmdList, DebugDrawFillMode::Solid, false, 1.0f, 1.0f, color, mSquarePyramid, pickingID);

                    debugDraw->drawCube(_cmdList, DebugDrawFillMode::Solid, true, (float3)-0.25f, (float3)0.25f, color, mCube, pickingID);
                    debugDraw->drawSquarePyramid(_cmdList, DebugDrawFillMode::Solid, true, 1.0f, 1.0f, color, mSquarePyramid, pickingID);
                }
            }
            break;
        }
    }
}