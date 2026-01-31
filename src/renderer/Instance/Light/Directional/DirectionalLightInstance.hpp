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

    VG_REGISTER_CLASS(DirectionalLightDesc, "Directional Light Desc");

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

    VG_REGISTER_CLASS(DirectionalLightInstance, "Directional Light Instance");

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
    bool DirectionalLightInstance::Cull(const ViewCullingOptions & _cullingOptions, CullingResult * _cullingResult)
    {
        // Directional light is never culled
        if (1)
        {
            super::Cull(_cullingOptions, _cullingResult);

            View * view = _cullingOptions.m_view;

            if (!view->isAdditionalView() && IsCastShadow())
            {
                VG_PROFILE_CPU("Add Shadow");

                // Create view and start culling immediately
                Renderer * renderer = Renderer::get();

                const Frustum & frustum = view->getCameraFrustum();

                // TODO: use some pooling to avoid recreating every frame
                ShadowView * shadowView = new ShadowView(this, view->getWorld(), getShadowResolution(), view);

                view->addShadowView(shadowView);

                float4x4 lightWorld = getGlobalMatrix();

                switch (m_shadowCameraMode)
                {
                    default:
                    case ShadowCameraMode::Fixed:
                    {
                        VG_PROFILE_CPU("Fixed");
                        shadowView->SetupOrthographicCamera(lightWorld, m_shadowSize, m_shadowRange);
                    }
                    break;

                    case ShadowCameraMode::FollowCameraTranslation:
                    {
                        VG_PROFILE_CPU("FollowCameraTranslation");
                        lightWorld[3].xyz += view->getViewInvMatrix()[3].xyz;
                        shadowView->SetupOrthographicCamera(lightWorld, m_shadowSize, m_shadowRange);
                    }
                    break;

                    case ShadowCameraMode::FitCameraFrustum:
                    {
                        VG_PROFILE_CPU("FitCameraFrustum");

                        // Find the bounding box in world-space
                        float3 frustumCenter = (float3)0.0f;

                        const auto & corners = frustum.getCorners();
                        for (uint i = 0; i < countof(corners); ++i)
                        {
                            const float3 corner = corners[i];
                            float3 p = corner.xyz;
                            frustumCenter += p;
                        }
                        frustumCenter /= (float)countof(corners);

                        float3 lightDir = -normalize(lightWorld[2].xyz);
                        float3 lightUp = lightWorld[1].xyz;

                        float3 eye = frustumCenter - lightDir * m_shadowRange.y;
                        float3 target = frustumCenter;

                        float4x4 lightView = View::lookAtRH(eye, target, lightUp);

                        float3 lightSpaceMin = (float3)FLT_MAX;
                        float3 lightSpaceMax = (float3)-FLT_MAX;

                        for (uint i = 0; i < countof(corners); ++i)
                        {
                            float4 p = mul(float4(corners[i], 1.0f), lightView);
                            lightSpaceMin = min(lightSpaceMin, p.xyz);
                            lightSpaceMax = max(lightSpaceMax, p.xyz);
                        }

                        float nearZ = lightSpaceMin.z;
                        float farZ = lightSpaceMax.z;

                        shadowView->SetupOrthographicCameraOffCenter(inverse(lightView), lightSpaceMin.x, lightSpaceMax.x, lightSpaceMin.y, lightSpaceMax.y, nearZ, farZ);
                    }
                    break;
                }

                _cullingResult->m_output->add(shadowView);

                shadowView->startJobs();                
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