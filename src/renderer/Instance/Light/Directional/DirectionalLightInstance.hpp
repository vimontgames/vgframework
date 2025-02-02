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

                ShadowView * shadowView = new ShadowView(this, _view->getWorld(), getShadowResolution());
                _view->addShadowView(shadowView);

                float4x4 shadowMatrix = this->getGlobalMatrix(); 
                shadowMatrix[0].xyz *= -1;
                shadowMatrix[1].xyz *= -1;
                shadowMatrix[2].xyz *= -1;
                //shadowMatrix[3].xyz *= -1;

                if (m_shadowCameraOffset)
                    shadowMatrix[3].xyz += _view->getViewInvMatrix()[3].xyz;

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
        super::Draw(_renderContext, _cmdList);

        switch (_renderContext.m_shaderPass)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_renderContext.m_shaderPass);
                break;

            case ShaderPass::ZOnly:
                break;

            case ShaderPass::Forward:
            case ShaderPass::Deferred:
            {
                const float4x4 & mGlobal = getGlobalMatrix();

                float4x4 mLine = float4x4::identity();
                         mLine[3].xyz = float3(0,0,0.5f);
                         mLine = mul(mLine, mGlobal);

                DebugDraw::get()->AddSolidCylinder(_renderContext.m_renderPass->getWorld(), 0.1f, 1.0f, packRGBA8(GetColor()), mLine, GetPickingID());

                float4x4 mArrow = float4x4::identity();
                         mArrow[0].xyz = float3(0.25f, 0.0f, 0.0f);
                         mArrow[1].xyz = float3(0.0f, 0.25f, 0.0f);
                         mArrow[2].xyz = float3(0.0f, 0.0f, 0.5f);
                         mArrow[3].xyz = float3(0, 0, 1.0f);
                         mArrow = mul(mArrow, mGlobal);

                DebugDraw::get()->AddSolidSquarePyramid(_renderContext.m_renderPass->getWorld(), 1.0f, 1.0f, packRGBA8(GetColor()), mArrow, GetPickingID());
            }
            break;
        }
    }
}