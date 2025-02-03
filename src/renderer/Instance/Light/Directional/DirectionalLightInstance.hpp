#include "renderer/Precomp.h"
#include "DirectionalLightInstance.h"
#include "Shaders/system/lightsBuffer.hlsli"
#include "renderer/View/Shadow/ShadowView.h"
#include "core/Scheduler/Scheduler.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // DirectionalLightDesc
    //--------------------------------------------------------------------------------------

    VG_REGISTER_OBJECT_CLASS(DirectionalLightDesc, "Directional Light Desc");

    //--------------------------------------------------------------------------------------
    bool DirectionalLightDesc::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    DirectionalLightDesc::DirectionalLightDesc(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    DirectionalLightDesc::~DirectionalLightDesc()
    {

    }

    //--------------------------------------------------------------------------------------
    // DirectionalLightInstance
    //--------------------------------------------------------------------------------------

    VG_REGISTER_OBJECT_CLASS(DirectionalLightInstance, "Directional Light Instance");

    //--------------------------------------------------------------------------------------
    bool DirectionalLightInstance::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    DirectionalLightInstance::DirectionalLightInstance(const DirectionalLightDesc * _directionalLightDesc) :
        super("Directional", nullptr, _directionalLightDesc)
    {
        
    }

    //--------------------------------------------------------------------------------------
    DirectionalLightInstance::~DirectionalLightInstance()
    {

    }

    //--------------------------------------------------------------------------------------
    bool DirectionalLightInstance::TryGetAABB(AABB & _aabb) const
    {
        // Directional light has no bounding volume
        return false;
    }

    //--------------------------------------------------------------------------------------
    // This method is called once for each view, thus we can compute a view-dependent matrix for directional lights
    //--------------------------------------------------------------------------------------
    bool DirectionalLightInstance::Cull(CullingResult * _cullingResult, View * _view) const
    {
        // Directional light is never culled
        if (1)
        {
            super::Cull(_cullingResult, _view);

            if (!_view->isAdditionalView() && IsCastShadow())
            {
                // Create view and start culling immediately
                Renderer * renderer = Renderer::get();

                const Frustum & frustum = _view->getCameraFrustum();

                ShadowView * shadowView = new ShadowView(this, _view->getWorld(), getShadowResolution());
                _view->addShadowView(shadowView);

                float4x4 shadowMatrix = this->getGlobalMatrix(); 
                shadowMatrix[0].xyz *= -1;
                shadowMatrix[1].xyz *= -1;
                shadowMatrix[2].xyz *= -1;

                if (m_shadowCameraOffset)
                    shadowMatrix[3].xyz = _view->getViewInvMatrix()[3].xyz;

                // Find the bounding box in world-space
                float3 minBounds = (float3)FLT_MAX;
                float3 maxBounds = (float3)-FLT_MAX;

                const auto & corners = frustum.getCorners();
                for (uint i = 0; i < countof(corners); ++i)
                {
                    const float3 corner = corners[i];
                    float3 p = mul(float4(corner.xyz, 1.0f), shadowMatrix).xyz;

                    minBounds = min(minBounds, p);
                    maxBounds = max(maxBounds, p);
                }

                shadowView->SetupOrthographicCamera(shadowMatrix, m_shadowSize, m_shadowRange);

                _cullingResult->m_output->add(shadowView);

                core::Scheduler * jobScheduler = (core::Scheduler *)Kernel::getScheduler();
                jobScheduler->Start(shadowView->getCullingJob(), renderer->GetJobSync(RendererJobType::Culling));
            }

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void DirectionalLightInstance::Draw(const RenderContext & _renderContext, gfx::CommandList * _cmdList) const
    {
        switch (_renderContext.m_shaderPass)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_renderContext.m_shaderPass);
                break;

            case ShaderPass::ZOnly:
                break;

            case ShaderPass::Outline:
            case ShaderPass::Forward:
            case ShaderPass::Deferred:
            {
                const float4x4 & mGlobal = getGlobalMatrix();
                auto * debugDraw = DebugDraw::get();
                const auto color = packRGBA8(GetColor());
                const auto pickingID = GetPickingID();

                float4x4 mLine = float4x4::identity();
                         mLine[0].xyz = float3(0.25f, 0.0f, 0.0f);
                         mLine[1].xyz = float3(0.0f, 0.25f, 0.0f);
                         mLine[2].xyz = float3(0.0f, 0.0f, 1.0f);
                         mLine[3].xyz = float3(0,0,0.5f);
                         mLine = mul(mLine, mGlobal);                

                float4x4 mArrow = float4x4::identity();
                         mArrow[0].xyz = float3(0.25f, 0.0f, 0.0f);
                         mArrow[1].xyz = float3(0.0f, 0.25f, 0.0f);
                         mArrow[2].xyz = float3(0.0f, 0.0f, 0.5f);
                         mArrow[3].xyz = float3(0, 0, 1.0f);
                         mArrow = mul(mArrow, mGlobal);

                if (ShaderPass::Outline == _renderContext.m_shaderPass)
                {
                    debugDraw->drawCube(_cmdList, DebugDrawFillMode::Outline, true, (float3)-0.25f, (float3)0.25f, color, mLine, pickingID);
                    debugDraw->drawSquarePyramid(_cmdList, DebugDrawFillMode::Outline, true, 1.0f, 1.0f, color, mArrow, pickingID);
                }
                else
                {
                    debugDraw->drawCube(_cmdList, DebugDrawFillMode::Solid, false, (float3)-0.25f, (float3)0.25f, color, mLine, pickingID);
                    debugDraw->drawSquarePyramid(_cmdList, DebugDrawFillMode::Solid, false, 1.0f, 1.0f, color, mArrow, pickingID);

                    debugDraw->drawCube(_cmdList, DebugDrawFillMode::Solid, true, (float3)-0.25f, (float3)0.25f, color, mLine, pickingID);
                    debugDraw->drawSquarePyramid(_cmdList, DebugDrawFillMode::Solid, true, 1.0f, 1.0f, color, mArrow, pickingID);
                }
            }
            break;
        }
    }
}