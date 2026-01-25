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
        const auto * view = (IView *)_renderPassContext.getView();
        if (view->IsToolmode())
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
        const auto * renderer = Renderer::get();
        const auto * options = RendererOptions::get();
        auto * view = static_cast<const View *>(_renderPassContext.getView());

        u16 toolmodeRWBufferID = -1;
        if (view->IsToolmode())
        {
            Buffer * toolmodeRWBuffer = getRWBuffer(_renderPassContext.getFrameGraphID("ToolmodeRWBuffer"));
            toolmodeRWBufferID = toolmodeRWBuffer->getRWBufferHandle();
            _cmdList->clearRWBuffer(toolmodeRWBuffer, 0x0);
        }

        const uint2 viewSize = view->GetSize();

        // TODO: 'Environment' class?
        const auto * world = view->GetWorld();

        // Get PBR textures

        // BRDF Cook-Torrance split-sum LUT
        const Texture * specularBRDF = renderer->getSpecularBRDF();

        // Unfiltered cubemap
        const Texture * cubemap = (Texture *)world->GetEnvironmentCubemap();
        {
            if (cubemap && cubemap->getTexDesc().type != gfx::TextureType::TextureCube)
                VG_WARNING("[Environment] Texture \"%s\" used as environment map should use the \"Cubemap\" texture type", cubemap->GetName().c_str());
        }

        // Irradiance cubemap (fallback to unfiltered cubemap)
        const Texture * irradianceCubemap = nullptr; 
        {
            if (asBool(PBRFlags::GenerateIrradianceCubemap & options->getPBRFlags()))
                irradianceCubemap = (Texture *)world->GetIrradianceCubemap();
            else
                irradianceCubemap = cubemap;
        }
        
        // Specular reflection cubemap (fallback to unfiltered cubemap)
        const Texture * specularReflectionCubemap = nullptr;
        {
            if (asBool(PBRFlags::GenerateSpecularReflectionCubemap & options->getPBRFlags()))
                specularReflectionCubemap = (Texture *)world->GetSpecularReflectionCubemap();
            else
                specularReflectionCubemap = cubemap;
        }

        ViewConstants * constants = (ViewConstants*)_cmdList->map(s_ViewConstantsBuffer, sizeof(ViewConstants)).data;
        {
            constants->setScreenSize(viewSize);
            constants->setMousePos((uint2)view->GetRelativeMousePos());
            constants->setDisplayMode(options->getDisplayMode());
            constants->setDisplayFlags(options->getDisplayFlags());
            constants->setToolmodeRWBufferID(toolmodeRWBufferID);
            constants->setCameraNearFar(view->getCameraNearFar());
            constants->setCameraFieldOfView(view->getCameraFovY());
            constants->setCameraAspectRatio((float)viewSize.x / (float)viewSize.y);
            constants->setView(view->getViewMatrix());
            constants->setViewInv(view->getViewInvMatrix());
            constants->setProj(view->getProjMatrix());
            constants->setProjInv(view->getProjInvMatrix());
            constants->setTLASHandle(view->getTLASHandle());
            constants->setEnvironmentColor(pow(world->GetEnvironmentColor().rgb, 2.2f));
            constants->setEnvironmentCubemap(cubemap ? cubemap->getTextureHandle() : ReservedSlot::InvalidTextureCube);
            constants->setSpecularBRDF(specularBRDF ? specularBRDF->getTextureHandle() : ReservedSlot::InvalidTexture2D);
            constants->setIrradianceCubemap(irradianceCubemap ? irradianceCubemap->getTextureHandle() : ReservedSlot::InvalidTextureCube);
            constants->setSpecularReflectionCubemap(specularReflectionCubemap ? specularReflectionCubemap->getTextureHandle() : ReservedSlot::InvalidTextureCube);
            constants->setIrradianceIntensity(world->GetIrradianceIntensity());
            constants->setSpecularReflectionIntensity(world->GetSpecularReflectionIntensity());

            const auto * camSettings = view->GetCameraSettings();
            if (camSettings)
            {
                constants->setFocalLength(camSettings->GetFocalLength() * 0.001f); // focal length in mm
                constants->setAperture(camSettings->GetAperture());
                constants->setFocusDistance(camSettings->GetFocusDistance());

                float dofScale = 1.0f;
                const float2 referenceResolution = float2(1920, 1080);
                switch (camSettings->GetGateFitMode())
                {
                    case GateFitMode::Horizontal:
                        dofScale = referenceResolution.x / ((float)viewSize.x);
                        break;

                    case GateFitMode::Vertical:
                        dofScale = referenceResolution.y / ((float)viewSize.y);
                        break;
                }
                constants->setDOFScale(dofScale);
            }

            if (view->IsOutlinePassNeeded())
            {
                const auto & outlines = options->getOutlineOptions();
                for (uint i = 0; i < OUTLINE_MASK_CATEGORIES_MAX; ++i)
                {
                    if (i < outlines.size())
                    {
                        const auto & categoryOptions = outlines[i];
                        constants->setOutlineColors(i, categoryOptions.getZPassOutlineColor(), categoryOptions.getZFailOutlineColor(), categoryOptions.getZFailFillColor());
                    }
                    else
                    {
                        constants->setOutlineColors(i, (float4)0.0f, (float4)0.0f, (float4)0.0f);
                    }
                }
            }

            //if (view->IsToolmode())
            //    VG_INFO("[Picking %s] RelativeMousePos = %i %i OVER = %s", _renderPassContext.m_view->getName().c_str(), (uint)constants->getMousePos().x, (uint)constants->getMousePos().y, _renderPassContext.m_view->IsMouseOverView() ? "true" : "false");
        }
        _cmdList->unmap(s_ViewConstantsBuffer);

        if (view->IsLit())
            updateLightsConstants(_renderPassContext, _cmdList);
    }
    
    //--------------------------------------------------------------------------------------
    void ViewConstantsUpdatePass::updateLightsConstants(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList)
    {
        VG_PROFILE_CPU("Lights");

        const auto view = static_cast<const View *>(_renderPassContext.getView()); 
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
            if (lights.m_instances.size() > 0)
            {
                useDefaultLight = false;
                break;
            }
        }
        const auto actualDirectionalCount = useDefaultLight ? 1 : directionalCount;

        const uint mapSize = sizeof(LightsConstantsHeader) + actualDirectionalCount * sizeof(DirectionalLightConstants) + omniCount * sizeof(OmniLightConstants);

        uint offset = 0;
        u8 * data = (u8*)_cmdList->map(s_LightsConstantsBuffer, mapSize).data;
        VG_ASSERT_IS_ALIGNED(data, 32);
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
                constants->setDirection(normalize(float3(0.5f,-0.75f,1)));
                constants->setShadowBias(0);
                constants->setShadowIntensity(0);
                constants->setShadowFarDist(0);
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
                #ifdef __clang__
                // temp workaround for clang issue in HLSL++
                constants->setDirection(-1.0f * directional->getGlobalMatrix()[2].xyz);
                #else
                constants->setDirection(-directional->getGlobalMatrix()[2].xyz);
                #endif
                constants->setShadowBias(directional->m_shadowBias);
                constants->setShadowIntensity(directional->m_shadowIntensity);
                constants->setShadowFarDist(directional->m_shadowRange.y);

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
                constants->setPosition(omni->getGlobalMatrix()[3].xyz);
                constants->setRadius(omni->getMaxRadius());
                constants->setShadowBias(omni->m_shadowBias);
                constants->setShadowIntensity(omni->m_shadowIntensity);

                offset += sizeof(OmniLightConstants);
            }            
        }
        _cmdList->unmap(s_LightsConstantsBuffer);
    }
}
