#include "ViewConstantsUpdatePass.h"
#include "shaders/system/view.hlsli"
#include "Shaders/system/toolmode.hlsl.h"
#include "Shaders/system/lightsBuffer.hlsli"
#include "renderer/Instance/Light/Directional/DirectionalLightInstance.h"
#include "renderer/Instance/Light/Omni/OmniLightInstance.h"

namespace vg::renderer
{
    gfx::Buffer * ViewConstantsUpdatePass::s_ViewConstantsBuffer = nullptr;
    gfx::Buffer * ViewConstantsUpdatePass::s_LightsConstantsBuffer = nullptr;

    static const uint s_LightBufferSize = 4096;

    //--------------------------------------------------------------------------------------
    ViewConstantsUpdatePass::ViewConstantsUpdatePass() :
        UpdatePass("ViewConstantsUpdatePass")
    {
        auto * device = Device::get();

        if (nullptr == s_ViewConstantsBuffer)
        {
            BufferDesc viewConstantsBufferDesc = BufferDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::Write, BufferFlags::None, sizeof(ViewConstants));
            s_ViewConstantsBuffer = device->createBuffer(viewConstantsBufferDesc, "ViewConstants", nullptr, ReservedSlot::ViewConstantsBufSrv);
        }
        else
        {
            VG_SAFE_INCREASE_REFCOUNT(s_ViewConstantsBuffer);
        }

        if (nullptr == s_LightsConstantsBuffer)
        {
            BufferDesc lightsConstantsBufferDesc = BufferDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::Write, BufferFlags::None, s_LightBufferSize);
            s_LightsConstantsBuffer = device->createBuffer(lightsConstantsBufferDesc, "LightsConstants", nullptr, ReservedSlot::LightsConstantBufSrv);
        }
        else
        {
            VG_SAFE_INCREASE_REFCOUNT(s_LightsConstantsBuffer);
        }
    }

    //--------------------------------------------------------------------------------------
    ViewConstantsUpdatePass::~ViewConstantsUpdatePass()
    {
        if (s_ViewConstantsBuffer && !s_ViewConstantsBuffer->Release())
            s_ViewConstantsBuffer = nullptr;

        if (s_LightsConstantsBuffer && !s_LightsConstantsBuffer->Release())
            s_LightsConstantsBuffer = nullptr;
    }

    //--------------------------------------------------------------------------------------
    void ViewConstantsUpdatePass::Setup(const gfx::RenderPassContext & _renderPassContext)
    {
        if (_renderPassContext.m_view->IsToolmode())
        {
            FrameGraphBufferResourceDesc toolmodeRWBufferDesc;
            toolmodeRWBufferDesc.elementSize = sizeof(ToolmodeRWBufferData);
            toolmodeRWBufferDesc.elementCount = 1;
        
            const auto toolmodeRWBufferID = _renderPassContext.getFrameGraphID("ToolmodeRWBuffer");
            createRWBuffer(toolmodeRWBufferID, toolmodeRWBufferDesc);
            writeRWBuffer(toolmodeRWBufferID); // declare write because we only want to be able to access the RWBuffer to get its RWBufferHandle 
        }
    }

    //--------------------------------------------------------------------------------------
    void ViewConstantsUpdatePass::BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList)
    {
        const auto options = RendererOptions::get();

        u16 toolmodeRWBufferID = -1;
        if (_renderPassContext.m_view->IsToolmode())
        {
            Buffer * toolmodeRWBuffer = getRWBuffer(_renderPassContext.getFrameGraphID("ToolmodeRWBuffer"));
            toolmodeRWBufferID = toolmodeRWBuffer->getRWBufferHandle();
            _cmdList->clearRWBuffer(toolmodeRWBuffer, 0x0);
        }

        View * view = (View *)_renderPassContext.m_view;
        const uint2 screenSize = view->getSize();

        ViewConstants * constants = (ViewConstants*)_cmdList->map(s_ViewConstantsBuffer, sizeof(ViewConstants)).data;
        {
            constants->setScreenSize(screenSize);
            constants->setMousePos((uint2)view->GetRelativeMousePos());
            constants->setDisplayMode(options->getDisplayMode());
            constants->setDisplayFlags(options->getDisplayFlags());
            constants->setToolmodeRWBufferID(toolmodeRWBufferID);
            constants->setCameraNearFar(view->getCameraNearFar());
            constants->setCameraFieldOfView(view->getCameraFovY());
            constants->setCameraAspectRatio((float)screenSize.x / (float)screenSize.y);
            constants->setView(view->getViewMatrix());
            constants->setViewInv(view->getViewInvMatrix());
            constants->setProj(view->getProjMatrix());
            constants->setProjInv(view->getProjInvMatrix());
            constants->setTLASHandle(view->getTLASHandle());
        }
        _cmdList->unmap(s_ViewConstantsBuffer);

        if (_renderPassContext.m_view->IsLit())
            updateLightsConstants(_renderPassContext, _cmdList);
    }

    //--------------------------------------------------------------------------------------
    void ViewConstantsUpdatePass::updateLightsConstants(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList)
    {
        VG_PROFILE_CPU("Lights");

        const auto view = (View *)_renderPassContext.m_view; 
        const ViewCullingJobOutput & culling = view->getCullingJobResult();

        const auto & directionals = culling.get(LightType::Directional).m_instances;
        const auto & omnis = culling.get(LightType::Omni).m_instances;
        const auto & spots = culling.get(LightType::Spot).m_instances;

        uint directionalCount = (uint)directionals.size();
        uint omniCount = (uint)omnis.size();
        uint spotCount = 0;// (uint)spots.size();

        // Add default light if needed
        bool useDefaultLight = true;
        for (uint i = 0; i < enumCount<LightType>(); ++i)
        {
            const auto lightType = (LightType)i;
            auto & lights = culling.get(lightType);
            if (lights.m_instances.count() > 0)
            {
                useDefaultLight = false;
                break;
            }
        }
        const auto actualDirectionalCount = useDefaultLight ? 1 : directionalCount;

        const uint mapSize = sizeof(LightsConstantsHeader) + actualDirectionalCount * sizeof(DirectionalLightConstants) + omniCount * sizeof(OmniLightConstants);

        uint offset = 0;
        u8 * data = (u8*)_cmdList->map(s_LightsConstantsBuffer, mapSize).data;
        {
            auto * header = (LightsConstantsHeader*)data;

            header->setDirectionalCount(actualDirectionalCount);
            header->setOmniCount(omniCount);
            header->setSpotCount(spotCount);

            offset += sizeof(LightsConstantsHeader);

            if (useDefaultLight)
            {
                DirectionalLightConstants * constants = (DirectionalLightConstants *)(data + offset);

                constants->setColor(float3(0.95f, 0.95f, 0.95f));
                constants->setAmbient(float3(0.05f, 0.05f, 0.05f));
                constants->setDirection(normalize(float3(1,-1,1)));
                constants->setShadowBias(0);
                constants->setShadowInstensity(0);
                constants->setShadowMapTextureHandle(0);
                constants->setShadowMatrix(float4x4::identity());

                offset += sizeof(DirectionalLightConstants);
            }            

            for (uint i = 0; i < directionalCount; ++i)
            {
                VG_ASSERT(LightType::Directional == directionals[i]->GetLightType());

                const DirectionalLightInstance * directional = (const DirectionalLightInstance *)directionals[i];
                DirectionalLightConstants * constants = (DirectionalLightConstants *)(data + offset);

                constants->setColor(directional->getColor().rgb * directional->getIntensity());
                constants->setAmbient(directional->getAmbient().rgb * directional->getIntensity());
                constants->setDirection(directional->getGlobalMatrix()[2].xyz);
                constants->setShadowBias(directional->m_shadowBias);
                constants->setShadowInstensity(directional->m_shadowIntensity);

                auto shadowView = view->findShadowView(directionals[i]);
                if (shadowView && shadowView->isUsingShadowMap())
                {
                    auto shadowMapID = getDepthStencil(shadowView->getShadowMapName(view))->getDepthTextureHandle();

                    constants->setShadowMapTextureHandle(shadowMapID);
                    constants->setShadowMatrix(shadowView->getViewProjMatrix());
                }
                else
                {
                    constants->setShadowMapTextureHandle(0);
                    constants->setShadowMatrix(float4x4::identity());
                }

                offset += sizeof(DirectionalLightConstants);
            }

            for (uint i = 0; i < omniCount; ++i)
            {
                VG_ASSERT(LightType::Omni == omnis[i]->GetLightType());

                const OmniLightInstance * omni = (const OmniLightInstance *)omnis[i];
                OmniLightConstants * constants = (OmniLightConstants *)(data + offset);

                constants->setColor(omni->getColor().rgb * omni->getIntensity());
                constants->setAmbient(omni->getAmbient().rgb * omni->getIntensity());
                constants->setPosition(omni->getGlobalMatrix()[3].xyz);
                constants->setRadius(omni->getMaxRadius());
                constants->setShadowBias(omni->m_shadowBias);
                constants->setShadowInstensity(omni->m_shadowIntensity);

                auto shadowMapID = ((View *)_renderPassContext.m_view)->findShadowMapID(omnis[i]);

                offset += sizeof(OmniLightConstants);
            }            
        }
        _cmdList->unmap(s_LightsConstantsBuffer);
    }
}
