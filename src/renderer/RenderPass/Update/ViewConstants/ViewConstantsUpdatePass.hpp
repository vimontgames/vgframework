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
    void ViewConstantsUpdatePass::Setup(const gfx::RenderPassContext & _renderPassContext, float _dt)
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
            constants->setMousePos(view->GetRelativeMousePos());
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

        updateLightsConstants(_renderPassContext, _cmdList);
    }

    //--------------------------------------------------------------------------------------
    void ViewConstantsUpdatePass::updateLightsConstants(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList)
    {
        View * view = (View*)_renderPassContext.m_view;
        const ViewCullingJobOutput & culling = view->getCullingJobResult();
        const auto & lights = culling.m_instanceLists[asInteger(GraphicInstanceListType::Light)].m_instances;

        uint mapSize = sizeof(LightsConstantsHeader);
        uint directionalCount = 0;
        uint omniCount = 0;

        for (uint i = 0; i < lights.size(); ++i)
        {
            LightInstance * light = (LightInstance*)lights[i];
            switch (light->GetLightType())
            {
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(light->GetLightType());
                break;

                case LightType::Directional:
                    directionalCount++;
                    break;

                case LightType::Omni:
                    omniCount++;
                    break;
            }
        }

        mapSize += directionalCount * sizeof(DirectionalLightConstants) + omniCount * sizeof(OmniLightConstants);

        uint offset = 0;
        u8 * data = (u8*)_cmdList->map(s_LightsConstantsBuffer, mapSize).data;
        {
            auto * header = (LightsConstantsHeader*)data;

            header->setDirectionalCount(directionalCount);
            header->setOmniCount(omniCount);

            offset += sizeof(LightsConstantsHeader);

            for (uint j = 0; j < enumCount<LightType>(); ++j)
            {
                for (uint i = 0; i < lights.size(); ++i)
                {
                    LightInstance * light = (LightInstance *)lights[i];
                    auto lightType = light->GetLightType();

                    // TODO : sort by LightType during culling instead
                    if (lightType == (LightType)j)
                    {
                        switch (lightType)
                        {
                            VG_ASSERT_ENUM_NOT_IMPLEMENTED(light->GetLightType());
                            break;

                            case LightType::Directional:
                            {
                                auto * directional = (DirectionalLightInstance *)light;
                                DirectionalLightConstants * constants = (DirectionalLightConstants *)(data + offset);

                                constants->setColor(directional->getColor().rgb * directional->getIntensity());
                                constants->setDirection(directional->getGlobalMatrix()[0].xyz);

                                offset += sizeof(DirectionalLightConstants);
                            }
                            break;

                            case LightType::Omni:
                            {
                                auto * omni = (OmniLightInstance *)light;
                                OmniLightConstants * constants = (OmniLightConstants *)(data + offset);

                                constants->setColor(omni->getColor().rgb * omni->getIntensity());
                                constants->setPosition(omni->getGlobalMatrix()[3].xyz);
                                constants->setRadius(omni->getMaxRadius());

                                offset += sizeof(OmniLightConstants);
                            }
                            break;
                        }
                    }
                }
            }
        }
        _cmdList->unmap(s_LightsConstantsBuffer);
    }
}
