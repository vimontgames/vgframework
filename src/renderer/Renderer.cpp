#pragma once

#include "renderer/Precomp.h"
#include "renderer.h"

#include "core/Kernel.h"
#include "core/Timer/Timer.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/File/File.h"
#include "core/Scheduler/Scheduler.h"
#include "core/string/string.h"

#include "gfx/device/device.h"
#include "gfx/Shader/ShaderManager.h"
#include "gfx/FrameGraph/FrameGraph.h"
#include "gfx/Profiler/Profiler.h"
#include "gfx/Resource/Texture.h"
#include "gfx/Importer/TextureImporter.h"

#include "renderer/RenderPass/ImGui/ImGui.h"
#include "renderer/RenderPass/ImGui/ImGuiPass.h"
#include "renderer/RenderPass/ImGui/imguiAdapter.h"
#include "renderer/RenderPass/Update/GPUDebug/GPUDebugUpdatePass.h"
#include "renderer/RenderPass/Update/InstanceData/InstanceDataUpdatePass.h"
#include "renderer/RenderPass/Compute/ComputeSkinning/ComputeSkinningPass.h"
#include "renderer/RenderPass/Update/BLAS/BLASUpdatePass.h"
#include "renderer/RenderPass/Render2D/HDROutput/HDROutputPass.h"
#include "renderer/RenderPass/Render2D/Preview/Texture/TexturePreviewPass.h"
#include "renderer/RenderPass/Compute/ComputeSpecularBRDF/ComputeSpecularBRDFPass.h"
#include "renderer/RenderPass/Compute/ComputeIBLCubemaps/ComputeIBLCubemapsPass.h"
#include "renderer/Importer/SceneImporterData.h"
#include "renderer/Model/Mesh/MeshModel.h"
#include "renderer/Animation/SkeletalAnimation.h"
#include "renderer/Options/RendererOptions.h"
#include "renderer/IGraphicInstance.h"
#include "renderer/Importer/TextureImporterData.h"
#include "renderer/Viewport/Viewport.h"
#include "renderer/View/View.h"
#include "renderer/View/Lit/LitView.h"
#include "renderer/Model/Material/DefaultMaterial/DefaultMaterialModel.h"
#include "renderer/Picking/PickingManager.h"
#include "renderer/DebugDraw/DebugDraw.h"
#include "renderer/RayTracing/RayTracingManager.h"
#include "renderer/Instance/Light/Directional/DirectionalLightInstance.h"
#include "renderer/Instance/Light/Omni/OmniLightInstance.h"
#include "renderer/UI/UIManager.h"

#if !VG_ENABLE_INLINE
#include "Renderer.inl"
#endif

// TODO : parse shader techniques & flag names from .hlsli
#include "shaders/driver/driver.hlsl.h"
#include "shaders/debugdraw/debugdraw.hlsl.h"
#include "shaders/default/default.hlsl.h"
#include "shaders/background/background.hlsl.h"
#include "shaders/postprocess/postprocess.hlsl.h"
#include "shaders/skinning/skinning.hlsl.h"
#include "shaders/lighting/deferredLighting.hlsl.h"
#include "shaders/preview/preview.hlsl.h"
#include "shaders/lighting/PrecomputeIBL.hlsl.h"

using namespace vg::core;
using namespace vg::gfx;
using namespace vg::renderer;

#define FBX_IMPORTER_SDK    0
#define FBX_IMPORTER_UFBX   1

#define FBX_IMPORTER FBX_IMPORTER_UFBX

#if FBX_IMPORTER == FBX_IMPORTER_SDK
#include "renderer/Importer/FBX/FBXSDKImporter/FBXSDKImporter.h"
#elif FBX_IMPORTER == FBX_IMPORTER_UFBX
#include "renderer/Importer/FBX/UFBXImporter/UFBXImporter.h"
#endif

//--------------------------------------------------------------------------------------
IRenderer * CreateNew()
{
	return new Renderer("Renderer", nullptr);
}

//--------------------------------------------------------------------------------------
namespace vg::renderer
{
    #ifdef _WIN32
    //--------------------------------------------------------------------------------------
    LRESULT CALLBACK Renderer::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
            return true;

        return 0;
    }
    #endif

    //--------------------------------------------------------------------------------------
    core::IProfiler * Renderer::GetProfiler() const
    {
        return Kernel::getProfiler();
    }

    //--------------------------------------------------------------------------------------
    IImGuiAdapter * Renderer::GetImGuiAdapter() const
    {
        return m_imgui;
    }

    //--------------------------------------------------------------------------------------
    gfx::IShaderManager * Renderer::GetShaderManager() const
    {
        return ShaderManager::get();
    }

	//--------------------------------------------------------------------------------------
	Renderer::Renderer(const core::string & _name, core::IObject * _parent) :
        IRenderer(_name, _parent),
		m_device(*(new Device())),
		m_frameGraph(*(new FrameGraph())),
        m_fbxImporter(nullptr)
	{
        #if FBX_IMPORTER == FBX_IMPORTER_SDK
        m_fbxImporter = new FBXSDKImporter();
        #elif FBX_IMPORTER == FBX_IMPORTER_UFBX
        m_fbxImporter = new UFBXImporter();
        #endif

        // Profiler instance has to be created by the graphic engine so as to profile the GPU too
        Kernel::setProfiler(new Profiler());
	}

	//--------------------------------------------------------------------------------------
	Renderer::~Renderer()
	{
        VG_SAFE_DELETE(m_fbxImporter);
		m_device.Release();
        IProfiler * profiler = Kernel::getProfiler();
        VG_SAFE_DELETE(profiler);
	}

    //--------------------------------------------------------------------------------------
    bool Renderer::RegisterClasses()
    {
        IFactory * factory = Kernel::getFactory();

        // Register classes to auto-register from the "Renderer" module
        AutoRegisterClassInfo::registerClasses(*factory);

        if (IClassDesc * desc = factory->registerPlugin(Renderer, "Renderer"))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Renderer::UnregisterClasses()
    {
        return AutoRegisterClassInfo::unregisterClasses();
    }

    //--------------------------------------------------------------------------------------
    bool Renderer::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    core::WinHandle g_winHandle = nullptr;

	//--------------------------------------------------------------------------------------
	void Renderer::Init(const RendererCreationParams & _params, core::Singletons & _singletons)
	{
        Timer::init();

        // Copy Profiler singleton created by the renderer
        _singletons.profiler = Kernel::getProfiler();

        // Get other Singletons for Renderer
        Kernel::setSingletons(_singletons);

        // Create device
		m_device.init(_params.device);

        // Picking
        m_picking = new PickingManager();

        // UI
        m_imgui = new ImGuiAdapter(_params.device.window, m_device);
        g_winHandle = _params.device.window;

        registerShaders();

        // Create Debug draw singleton
        new DebugDraw();

        // Create UIManager singleton
        new UIManager();

        // Create passes not bound to a View
        m_gpuDebugUpdatePass = new GPUDebugUpdatePass();
        m_instanceDataUpdatePass = new InstanceDataUpdatePass();
        m_computeSkinningPass = new ComputeSkinningPass();
        m_computeSpecularBRDFPass = new ComputeSpecularBRDFPass();
        m_computeIBLCubemapsPass = new ComputeIBLCubemapsPass();
        m_BLASUpdatePass = new BLASUpdatePass();
        m_imguiPass = new ImGuiPass();
        m_hdrOutputPass = new HDROutputPass();

        RegisterClasses();

        RayTracingManager * rtManager = new RayTracingManager();

        RendererOptions * options = new RendererOptions("Renderer Options", this);

        initDefaultTextures();
        initDefaultMaterials();        

        // Shared job output (must be created before views because it's needed to init View culling jobs
        m_sharedCullingJobOutput = new SharedCullingJobOutput();

        // Create "Game" viewport for "Game" views
        auto gameViewportParams = gfx::CreateViewportParams(ViewportTarget::Game, GetBackbufferSize());
        if (auto gameViewport = (Viewport *)CreateViewport(gameViewportParams, "Game"))
        {
            AddViewport(gameViewport);
            VG_SAFE_RELEASE(gameViewport);
        }

        m_bakedSpecularBRDF = m_device.createTexture("data/Engine/BRDF/CookTorrance.png");
	}

    //--------------------------------------------------------------------------------------
    void Renderer::registerShaders()
    {
        auto * sm = ShaderManager::get();

        // TODO: register from parsing 
        sm->registerHLSL(DriverHLSLDesc());
        sm->registerHLSL(DebugDrawHLSLDesc());
        sm->registerHLSL(DefaultHLSLDesc());
        sm->registerHLSL(BackgroundHLSLDesc());
        sm->registerHLSL(PostProcessHLSLDesc());
        sm->registerHLSL(SkinningHLSLDesc());
        sm->registerHLSL(DeferredLightingHLSLDesc());
        sm->registerHLSL(PreviewHLSLDesc());
        sm->registerHLSL(PrecomputeIBLHLSLDesc());

        // Register callback for after shader updates
        m_device.getShaderManager()->setOnShadersUpdatedCallback(onShadersUpdated);

        sm->update(true);
    }

	//--------------------------------------------------------------------------------------
	void Renderer::Deinit()
	{
        VG_SAFE_RELEASE(m_bakedSpecularBRDF);
        VG_SAFE_RELEASE(m_generatedSpecularBRDF);
        VG_SAFE_DELETE(m_sharedCullingJobOutput);
        VG_SAFE_RELEASE(m_hdrOutput);

        UnregisterClasses();

        deinitDefaultMaterials();
        deinitDefaultTextures();

        RendererOptions * options = RendererOptions::get();
        VG_SAFE_DELETE(options);

        UIManager * uiManager = (UIManager*)GetUIManager();
        VG_SAFE_RELEASE(uiManager);

        DebugDraw * dbgDraw = getDebugDraw();
        VG_SAFE_RELEASE(dbgDraw);

        RayTracingManager * rtManager = RayTracingManager::get();
        VG_SAFE_RELEASE(rtManager);

        // Clear viewports
        for (uint j = 0; j < core::enumCount<gfx::ViewportTarget>(); ++j)
        {
            auto & viewports = m_viewports[j];
            for (uint i = 0; i < viewports.size(); ++i)
                VG_SAFE_RELEASE(viewports[i]);
            viewports.clear();
        }

        // Clear remaining views
        for (uint j = 0; j < core::enumCount<gfx::ViewTarget>(); ++j)
        {
            auto & views = m_views[j];
            for (uint i = 0; i < views.size(); ++i)
                VG_SAFE_RELEASE(views[i]);
            views.clear();
        }

        VG_SAFE_RELEASE(m_gpuDebugUpdatePass);
        VG_SAFE_RELEASE(m_instanceDataUpdatePass);
        VG_SAFE_DELETE(m_computeSkinningPass);
        VG_SAFE_DELETE(m_computeSpecularBRDFPass);
        VG_SAFE_DELETE(m_computeIBLCubemapsPass);
        VG_SAFE_DELETE(m_BLASUpdatePass);
        VG_SAFE_DELETE(m_imguiPass);
        VG_SAFE_DELETE(m_imgui);
        VG_SAFE_DELETE(m_hdrOutputPass);

        m_frameGraph.Release();

		m_device.deinit();

        VG_SAFE_RELEASE(m_picking);
	}

    //--------------------------------------------------------------------------------------
    void Renderer::CreateResources()
    {
        RendererOptions::get()->createResources();
    }

    //--------------------------------------------------------------------------------------
    void Renderer::ReleaseResources()
    {
        RendererOptions::get()->releaseResources();
    }

    //--------------------------------------------------------------------------------------
    void Renderer::SetResized()
    {
        m_frameGraph.setResized();
    }

    //--------------------------------------------------------------------------------------
    void Renderer::Resize(core::uint _width, core::uint _height)
    {
        m_device.waitGPUIdle();
        m_device.updateHDR();
        m_device.resize(_width, _height);

        if (IsFullscreen())
        {
            auto & viewports = m_viewports[(int)ViewportTarget::Game];
            for (uint i = 0; i < viewports.size(); ++i)
            {
                auto * viewport = viewports[i];
                if (nullptr != viewport)
                {
                    viewport->SetRenderTargetSize(GetBackbufferSize());
                    viewport->SetFocused(true);
                    viewport->SetVisible(true);
                }
            }
        }

        m_frameGraph.destroyTransientResources();
        DebugDraw::get()->reset();
    }

    //--------------------------------------------------------------------------------------
    void Renderer::Move() 
    {
        m_device.waitGPUIdle();
        if (m_device.updateHDR())
        {
            m_frameGraph.destroyTransientResources();
            DebugDraw::get()->reset();
        }
    }

    //--------------------------------------------------------------------------------------
    core::uint2 Renderer::GetBackbufferSize() const
    {
        const auto & desc = getBackbuffer()->getTexDesc();
        return { desc.width, desc.height };
    }

    //--------------------------------------------------------------------------------------
    // Because shader update is asynchronous, we pass a callback function to get notified about shaders being updated
    //--------------------------------------------------------------------------------------
    void Renderer::UpdateShaders()
    {
        m_device.getShaderManager()->update();
    }

    //--------------------------------------------------------------------------------------
    // Release specular BRDF to force recompute after shader update
    //--------------------------------------------------------------------------------------
    void Renderer::onShadersUpdated(bool _success)
    {
        auto * _this = Renderer::get();
        _this->releaseSpecularBRDF();
    }

    //--------------------------------------------------------------------------------------
    void Renderer::WaitGPUIdle()
    {
        m_device.waitGPUIdle();
    }

    //--------------------------------------------------------------------------------------
    void Renderer::SetVSync(gfx::VSync mode)
    {
        m_device.setVSync(mode);
    }

    //--------------------------------------------------------------------------------------
    gfx::VSync Renderer::GetVSync() const
    {
        return m_device.getVSync();
    }

    //--------------------------------------------------------------------------------------
    void Renderer::SetHDR(gfx::HDR _mode)
    {
        m_device.setHDR(_mode);
    }

    //--------------------------------------------------------------------------------------
    gfx::HDR Renderer::GetHDR() const
    {
        return m_device.getHDR();
    }

	//--------------------------------------------------------------------------------------
	void Renderer::RunOneFrame()
	{
        if (m_device.isMinimized())
        {
            Sleep(1);
            DebugDraw::get()->endFrame();
            return;
        }

        VG_PROFILE_CPU("Renderer");

        const RendererOptions * options = RendererOptions::get(false);
        if (!options)
            return;
        
        options->update();

		m_device.beginFrame();
		{
            //if (!m_fullscreen)
                m_imgui->beginFrame();

            //--------------------------------------------------------------------------------------
            // Culling all views : 
            // - a job a created for each view to fill View::m_cullingJobResult & synced
            // - and some results are merged to 
            //--------------------------------------------------------------------------------------
            cullViews();

            // Framegraph
            {
                VG_PROFILE_CPU("Framegraph");

                m_frameGraph.importRenderTarget("Backbuffer", m_device.getBackbuffer(), float4(0,0,0,1), FrameGraphResource::InitState::Clear);
                m_frameGraph.setGraphOutput("Backbuffer");

                const bool hdrOutput = HDR::None != GetHDR();
                if (hdrOutput)
                {
                    gfx::TextureDesc hdrOutputDesc( gfx::Usage::Default,
                                                    gfx::BindFlags::ShaderResource,
                                                    gfx::CPUAccessFlags::None,
                                                    gfx::TextureType::Texture2D,
                                                    gfx::PixelFormat::R16G16B16A16_float,
                                                    gfx::TextureFlags::RenderTarget,
                                                    GetBackbufferSize().x,
                                                    GetBackbufferSize().y);

                    if (nullptr == m_hdrOutput || m_hdrOutput->getTexDesc() != hdrOutputDesc)
                    {
                        VG_SAFE_RELEASE(m_hdrOutput);
                        m_hdrOutput = (Texture *)CreateTexture(hdrOutputDesc, "HDROutput");
                    }

                    m_frameGraph.importRenderTarget("HDROutput", m_hdrOutput, float4(0, 0, 0, 1), FrameGraphResource::InitState::Clear);
                }

                // Register passes not linked to views (e.g. skinning, or BLAS updates)
                RenderPassContext mainViewRenderPassContext;
                mainViewRenderPassContext.setView(nullptr);

                m_frameGraph.addUserPass(mainViewRenderPassContext, m_gpuDebugUpdatePass, "GPU Debug");

                m_frameGraph.addUserPass(mainViewRenderPassContext, m_instanceDataUpdatePass, "Instance Data");
                m_frameGraph.addUserPass(mainViewRenderPassContext, m_computeSkinningPass, "Skinning");

                if (options->isRayTracingEnabled())
                    m_frameGraph.addUserPass(mainViewRenderPassContext, m_BLASUpdatePass, "BLAS Update");

                if (asBool(PBRFlags::GenerateSpecularBRDF & options->getPBRFlags()))
                {
                    // Generate specular BRDF LUT if dirty
                    if (nullptr == m_generatedSpecularBRDF)
                    {
                        TextureDesc specularBRDFDesc = TextureDesc(Usage::Default, BindFlags::ShaderResource | BindFlags::UnorderedAccess, CPUAccessFlags::None, TextureType::Texture2D, PixelFormat::R16G16_float, TextureFlags::None, 256, 256, 1, 1, MSAA::None);
                        //specularBRDFDesc.mipmaps = 2; // test
                        m_generatedSpecularBRDF = m_device.createTexture(specularBRDFDesc, "SpecularBRDF_LUT");

                        m_computeSpecularBRDFPass->setSpecularBRDFTexture(m_generatedSpecularBRDF);
                        m_frameGraph.addUserPass(mainViewRenderPassContext, m_computeSpecularBRDFPass, "Compute Specular BRDF LUT");
                    }
                }
                else
                {
                    releaseSpecularBRDF();
                }

                // Register viewports 
                for (uint j = 0; j < core::enumCount<gfx::ViewportTarget>(); ++j)
                {
                    auto target = gfx::ViewportTarget(j);

                    if (m_fullscreen)
                    {
                        if (gfx::ViewportTarget::Editor == target)
                            continue;
                    }

                    for (uint i = 0; i < m_viewports[j].size(); ++i)
                    {
                        auto * viewport = m_viewports[j][i];
                        if (viewport->AnyRender())
                        {
                            auto * rt = (Texture *)viewport->GetRenderTarget();
                            if (rt)
                                m_frameGraph.importRenderTarget(rt->GetName(), rt, float4(0, 0, 0, 1), FrameGraphResource::InitState::Clear);
                        }
                    }
                }

                // Gather worlds from all visible views
                vector<IWorld *> visibleWorlds;
                vector<IView *> visibleViews;

                // Register view passes
                for (uint j = 0; j < core::enumCount<gfx::ViewTarget>(); ++j)
                {
                    auto target = gfx::ViewTarget(j);

                    if (m_fullscreen)
                    {
                        if (gfx::ViewTarget::Editor == target)
                            continue;
                    }

                    auto & views = m_views[j];
                    for (uint i = 0; i < views.size(); ++i)
                    {
                        auto * view = (View *)views[i];
                        if (nullptr != view)
                        {
                            if (!view->IsRender())
                                continue;

                            if (IWorld * world = view->GetWorld())
                            {
                                if (!vector_helper::exists(visibleWorlds, world))
                                    visibleWorlds.push_back(world);
                            }

                            visibleViews.push_back(view);
                        }
                    }
                }

                // TODO: detect the source envmap used to compute irradiance/specular cubemaps changed and compute it only once
                const PBRFlags pbrFlags = options->getPBRFlags();
                if (asBool((PBRFlags::GenerateIrradianceCubemap | PBRFlags::GenerateSpecularReflectionCubemap) & pbrFlags))
                {
                    for (uint i = 0; i < visibleWorlds.size(); ++i)
                    {
                        auto * world = visibleWorlds[i];

                        Texture * environmentCubemap = (Texture*)world->GetEnvironmentCubemap();

                        if (nullptr != environmentCubemap)
                        {
                            Texture * irradianceCubemap = nullptr;

                            if (asBool(PBRFlags::GenerateIrradianceCubemap & pbrFlags))
                            {
                                irradianceCubemap = (Texture *)world->GetIrradianceCubemap();

                                TextureDesc irradianceCubemapDesc = environmentCubemap->getTexDesc();
                                irradianceCubemapDesc.resource.m_bindFlags = BindFlags::ShaderResource | BindFlags::UnorderedAccess;
                                irradianceCubemapDesc.format = PixelFormat::R16G16B16A16_float;
                                irradianceCubemapDesc.width = 32;
                                irradianceCubemapDesc.height = 32;
                                irradianceCubemapDesc.mipmaps = 1;

                                if (nullptr != irradianceCubemap && irradianceCubemapDesc != irradianceCubemap->getTexDesc())
                                {
                                    world->SetIrradianceCubemap(nullptr); // release
                                    irradianceCubemap = nullptr;
                                }

                                if (nullptr == irradianceCubemap)
                                {
                                    irradianceCubemap = m_device.createTexture(irradianceCubemapDesc, "Irradiance cubemap");
                                    world->SetIrradianceCubemap(irradianceCubemap);
                                    VG_SAFE_RELEASE(irradianceCubemap);
                                }
                            }

                            Texture * specularReflectionCubemap = nullptr;
                            if (asBool(PBRFlags::GenerateSpecularReflectionCubemap & pbrFlags))
                            {
                                specularReflectionCubemap = (Texture *)world->GetSpecularReflectionCubemap();

                                TextureDesc specularReflectionCubemapDesc = environmentCubemap->getTexDesc();
                                specularReflectionCubemapDesc.resource.m_bindFlags = BindFlags::ShaderResource | BindFlags::UnorderedAccess;
                                specularReflectionCubemapDesc.format = PixelFormat::R16G16B16A16_float;

                                if (nullptr != specularReflectionCubemap && specularReflectionCubemapDesc != specularReflectionCubemap->getTexDesc())
                                {
                                    world->SetSpecularReflectionCubemap(nullptr); // release
                                    specularReflectionCubemap = nullptr;
                                }

                                if (nullptr == specularReflectionCubemap)
                                {
                                    specularReflectionCubemap = m_device.createTexture(specularReflectionCubemapDesc, "Specular Reflection cubemap");
                                    world->SetSpecularReflectionCubemap(specularReflectionCubemap);
                                    VG_SAFE_RELEASE(specularReflectionCubemap);
                                }
                            }
                         
                            m_computeIBLCubemapsPass->add(environmentCubemap, irradianceCubemap, specularReflectionCubemap);
                        }
                    }

                    m_frameGraph.addUserPass(mainViewRenderPassContext, m_computeIBLCubemapsPass, "Compute IBL Cubemaps");
                }

                for (uint i = 0; i < visibleViews.size(); ++i)
                {
                    View * view = (View *)visibleViews[i];

                    gfx::RenderPassContext rc;
                                           rc.setView(view);

                    view->RegisterFrameGraph(rc, m_frameGraph);
                }

                // Additional preview passes
                m_imgui->RegisterFrameGraph(mainViewRenderPassContext, m_frameGraph);

                m_frameGraph.addUserPass(mainViewRenderPassContext, m_imguiPass, "ImGui");

                if (hdrOutput)
                    m_frameGraph.addUserPass(mainViewRenderPassContext, m_hdrOutputPass, "HDROutput");

                m_frameGraph.setup();
                m_frameGraph.build();
                m_frameGraph.render();

                for (uint j = 0; j < core::enumCount<gfx::ViewTarget>(); ++j)
                {
                    auto & views = m_views[j];
                    for (uint i = 0; i < views.size(); ++i)
                    {
                        auto * view = (View *)views[i];
                        if (view)
                            view->clearShadowViews();
                    }
                }

                // reset debug draw after last render
                DebugDraw::get()->endFrame();
            }
		}
		m_device.endFrame();

        for (uint j = 0; j < core::enumCount<gfx::ViewTarget>(); ++j)
        {
            auto target = gfx::ViewTarget(j);

            // TODO: ShadowMaps too?
            if (target == ViewTarget::Game)
            {
                auto & views = m_views[j];
                for (uint i = 0; i < views.size(); ++i)
                {
                    auto * view = (View *)views[i];
                    if (nullptr != view)
                        view->SetRender(false);
                }
            }
        }
	}

    //--------------------------------------------------------------------------------------
    void Renderer::cullViews()
    {
        if (!m_sharedCullingJobOutput)
            return;

        m_sharedCullingJobOutput->clear();

        core::Scheduler * jobScheduler = (core::Scheduler *)Kernel::getScheduler();
        auto jobSync = GetJobSync(RendererJobType::Culling);

        // Perform culling for each view (might want to split views later)
        uint jobStartCounter = 0;
        for (uint j = 0; j < core::enumCount<gfx::ViewTarget>(); ++j)
        {
            const auto & views = m_views[j];
            for (uint i = 0; i < views.size(); ++i)
            {
                View * view = views[i];
                if (view && view->IsVisible())
                {
                    auto * job = view->getCullingJob();
                    jobScheduler->Start(job, jobSync);
                    jobStartCounter++;
                }
            }
        }

        if (jobStartCounter > 0)
        {
            VG_PROFILE_CPU("Sync Culling");
            jobScheduler->Wait(jobSync);
        }
    }

	//--------------------------------------------------------------------------------------
	Texture * Renderer::getBackbuffer() const
	{
		return m_device.getBackbuffer();
	}

    //--------------------------------------------------------------------------------------
    const gfx::DeviceParams & Renderer::GetDeviceCreationParams() const
    {
        return m_device.getDeviceParams();
    }

    //--------------------------------------------------------------------------------------
    renderer::IRendererOptions * Renderer::GetOptions()
    {
        return RendererOptions::get();
    }

    //--------------------------------------------------------------------------------------
    gfx::ITexture * Renderer::CreateTexture(const gfx::TextureDesc & _texDesc, const core::string & _name)
    {
        auto tex = m_device.createTexture(_texDesc, _name);
        tex->SetName(_name);
        return tex;
    }

    //--------------------------------------------------------------------------------------
    gfx::IViewport * Renderer::CreateViewport(const gfx::CreateViewportParams & _params, const core::string & _name, gfx::ViewportFlags _flags)
    {
        if (auto * viewport = new Viewport(_params))
        {
            viewport->SetName(_name);
            viewport->SetFlags(_flags);

            return viewport;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    gfx::ViewportID Renderer::AddViewport(gfx::IViewport * _viewport)
    {
        auto target = _viewport->GetViewportID().target;
        auto & viewports = m_viewports[(uint)target];

        // Find hole or push_back
        VG_ASSERT(viewports.size() < (ViewIndex)-1);
        VG_SAFE_INCREASE_REFCOUNT(_viewport);

        for (uint i = 0; i < viewports.size(); ++i)
        {
            if (!viewports[i])
            {
                viewports[i] = (Viewport *)_viewport;
                auto id = ViewportID(target, i);
                _viewport->SetViewportID(id);
                return id;
            }
        }

        auto index = (ViewportIndex)viewports.size();
        ViewportID id = ViewportID(target, index);
        viewports.push_back((Viewport *)_viewport);
        _viewport->SetViewportID(id);
        _viewport->SetName(fmt::sprintf("%s Viewport #%u", asString(id.target), id.index));

        return id;
    }

    //--------------------------------------------------------------------------------------
    gfx::ViewportIndex Renderer::GetFreeViewportIndex(gfx::ViewportTarget _target)
    {
        auto & viewports = m_viewports[(uint)_target];
        for (uint i = 0; i < viewports.size(); ++i)
        {
            if (!viewports[i])
                return i;
        }
        return (gfx::ViewportIndex)viewports.size();
    }

    //--------------------------------------------------------------------------------------
    gfx::IViewport * Renderer::GetViewport(gfx::ViewportID _viewportID)
    {
        const auto & viewports = m_viewports[asInteger(_viewportID.target)];
        if (_viewportID.index < viewports.size())
            return viewports[_viewportID.index];
        else
            return nullptr;
    }

    //--------------------------------------------------------------------------------------
    const core::vector<gfx::IViewport *> & Renderer::GetViewports(gfx::ViewportTarget _target) const
    {
        return (core::vector<gfx::IViewport *> &)m_viewports[asInteger(_target)];
    }

    //--------------------------------------------------------------------------------------
    gfx::IView * Renderer::CreateView(gfx::CreateViewParams _params, const core::string & _name, ViewFlags _flags)
    {
        View * view = nullptr;

        switch (_params.target)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_params.target);
                break;

            case ViewTarget::Game:
            case ViewTarget::Editor:
                view = new LitView(_params);
                break;
        }
        
        if (view)
        {
            view->SetName(_name);
            view->setFlags(_flags);
        }
        return view;
    }

    //--------------------------------------------------------------------------------------
    gfx::ViewID Renderer::AddView(gfx::IView * _view)
    {
        auto target = _view->GetViewID().target;
        auto & views = m_views[(uint)target];

        // Find hole or push_back
        VG_ASSERT(views.size() < (ViewIndex)-1);
        VG_SAFE_INCREASE_REFCOUNT(_view);

        for (uint i = 0; i < views.size(); ++i)
        {
            if (!views[i])
            {
                views[i] = (View *)_view;
                auto id = ViewID(target, i);
                _view->SetViewID(id);
                return id;
            }
        }

        auto index = (ViewIndex)views.size();
        ViewID id = ViewID(target, index);
        views.push_back((View *)_view);
        _view->SetViewID(id);
        _view->SetName(fmt::sprintf("%s View #%u", asString(id.target), id.index));

        return id;
    }

    //--------------------------------------------------------------------------------------
    const core::vector <IView *> Renderer::GetViews(gfx::ViewTarget _target) const
    {
        VG_ASSERT(_target < (gfx::ViewTarget)core::enumCount<gfx::ViewTarget>());
        return (const core::vector<IView *>&)m_views[(uint)_target];
    }

    //--------------------------------------------------------------------------------------
    void Renderer::RemoveView(gfx::ViewID _viewID)
    {
        // Keep holes so that the ViewID remains valid
        auto & views = m_views[(uint)_viewID.target];
        for (uint i = 0; i < views.size(); ++i)
        {
            if (views[i])
            {
                const auto id = views[i]->GetViewID();
                if (id.index == _viewID.index)
                    VG_SAFE_RELEASE(views[i]);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    gfx::ViewIndex Renderer::GetFreeViewIndex(gfx::ViewTarget _target) const
    {
        const auto & views = GetViews(_target);
        for (uint i = 0; i < views.size(); ++i)
        {
            if (!views[i])
                return i;
        }
        return (gfx::ViewIndex)views.size();
    }

    //--------------------------------------------------------------------------------------
    gfx::IView * Renderer::GetView(gfx::ViewID _viewID) const
    {
        if (gfx::ViewTargetInvalid != _viewID.target && gfx::ViewIndexInvalid != _viewID.index)
        {
            const auto & views = m_views[(uint)_viewID.target];
            if (_viewID.index < views.size())
                return views[_viewID.index];
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    gfx::IView * Renderer::GetView(gfx::ViewTarget _target, const core::IWorld * _world) const
    {
        VG_ASSERT(_world);
        if (nullptr == _world)
            return nullptr;

        const auto & views = GetViews(_target);
        for (uint i = 0; i < views.size(); ++i)
        {
            if (const auto * view = views[i])
            {
                if (view->GetWorld() == _world)
                    return (gfx::IView *)view;
            }
        }
        return nullptr;
    }
    
    //--------------------------------------------------------------------------------------
    bool Renderer::cookMeshModel(const core::string & _file)
    {
        SceneImporterData imported;

        if (m_fbxImporter->importFBX(_file, imported))
        {
            if (imported.meshes.size() > 0)
            {
                const auto & mesh = imported.meshes[0];
                mesh.save(io::getCookedPath(_file));
            }

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    IMeshModel * Renderer::loadMeshModel(const core::string & _file)
    {
        MeshImporterData meshData;

        if (meshData.load(io::getCookedPath(_file)))
        {
            IMeshModel * meshModel = MeshModel::createFromImporterData(meshData);
            return meshModel;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool Renderer::cookTexture(const core::string & _file, const gfx::TextureImporterSettings * _importSettings)
    {
        TextureImporterData imported;
        imported.name = _file;

        if (TextureImporter::get()->importTextureData(_file, imported.desc, imported.texels, _importSettings))
        {
            imported.save(io::getCookedPath(_file));

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    gfx::ITexture * Renderer::loadTexture(const core::string & _file)
    {
        TextureImporterData textureData;

        if (textureData.load(io::getCookedPath(_file)))
        {
            ITexture * texture = m_device.createTexture(textureData.desc, textureData.name, textureData.texels.data());
            return texture;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool Renderer::cookAnimation(const core::string & _file)
    {
        SceneImporterData imported;

        if (m_fbxImporter->importFBX(_file, imported))
        {
            if (imported.anims.size() > 0)
            {
                const auto & anim = imported.anims[0];
                anim.save(io::getCookedPath(_file));
            }

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    IAnimation * Renderer::loadAnimation(const core::string & _file) 
    {
        AnimImporterData animData;

        if (animData.load(io::getCookedPath(_file)))
        {
            IAnimation * animation = SkeletalAnimation::createFromImporterData(animData);
            return animation;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void Renderer::ReleaseAsync(core::IObject * _object)
    {
        VG_SAFE_RELEASE_ASYNC(_object);
    }

    //--------------------------------------------------------------------------------------
    void Renderer::initDefaultTextures()
    {
        m_defaultTextures.resize(enumCount<MaterialTextureType>());

        for (uint t = 0; t < m_defaultTextures.size(); ++t)
        {
            const auto type = (MaterialTextureType)t;
            u32 data;
            ReservedSlot reservedSlot = ReservedSlot::None;
            switch (type)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                    break;

                case MaterialTextureType::Albedo:
                    data = 0xFFBBBBBB;
                    reservedSlot = ReservedSlot::DefaultAlbedoTexSrv;
                    break;

                case MaterialTextureType::Normal:
                    data = 0xFFFF7F7F;
                    reservedSlot = ReservedSlot::DefaultNormalTexSrv;
                    break;

                case MaterialTextureType::PBR:
                    data = 0x00FFFFFF;    // Blue: Metalness Green: Roughness Red: Occlusion (will be multiplied by material constants)
                    reservedSlot = ReservedSlot::DefaultPBRTexSrv;
                    break;
            }

            TextureDesc texDesc = TextureDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, TextureType::Texture2D, PixelFormat::R8G8B8A8_unorm, TextureFlags::None, 1, 1);
            string name = "Default_" + asString(type);
            m_defaultTextures[t] = m_device.createTexture(texDesc, name.c_str(), &data, reservedSlot);
        }        
    }

    //--------------------------------------------------------------------------------------
    void Renderer::deinitDefaultTextures()
    {
        for (uint t = 0; t < m_defaultTextures.size(); ++t)
            VG_SAFE_RELEASE(m_defaultTextures[t]);

        m_defaultTextures.clear();
    }

    //--------------------------------------------------------------------------------------
    void Renderer::initDefaultMaterials()
    {
        VG_ASSERT(nullptr == m_defaultMaterial);
        m_defaultMaterial = new DefaultMaterialModel("Default Material", this);
    }

    //--------------------------------------------------------------------------------------
    void Renderer::deinitDefaultMaterials()
    {
        VG_SAFE_RELEASE(m_defaultMaterial);
    }

    //--------------------------------------------------------------------------------------
    void Renderer::SetFullscreen(bool _fullscreen)
    {
        #if !VG_ENABLE_EDITOR
        _fullscreen = true;
        #endif

        if (_fullscreen != m_fullscreen)
        {
            m_fullscreen = _fullscreen;
            VG_INFO("[Renderer] Set Fullscreen to '%s'", _fullscreen ? "true" : "false");

            if (_fullscreen)
            {
                // Resize viewports to fit backbuffer
                auto & viewports = m_viewports[(int)ViewportTarget::Game];
                for (uint i = 0; i < viewports.size(); ++i)
                {
                    auto * viewport = viewports[i];
                    if (nullptr != viewport)
                    {
                        viewport->SetRenderTarget(nullptr);
                        viewport->SetRenderTargetSize(GetBackbufferSize());
                        viewport->SetFocused(true);
                        viewport->SetVisible(true);
                    }
                }
            }
            else
            {
                // Hide all game view so as not to render the game views if it's not visible in the editor
                auto & viewports = m_viewports[(int)ViewportTarget::Game];
                for (uint i = 0; i < viewports.size(); ++i)
                {
                    auto * viewport = viewports[i];
                    if (nullptr != viewport)
                    {
                        viewport->SetFocused(false);
                        viewport->SetVisible(false);
                    }
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    bool Renderer::IsFullscreen() const
    {
        #if !VG_ENABLE_EDITOR
        return true;
        #else
        return m_fullscreen;
        #endif
    }

    //--------------------------------------------------------------------------------------
    IPicking * Renderer::GetPicking() const
    {
        return m_picking;
    }

    //--------------------------------------------------------------------------------------
    IDebugDraw * Renderer::GetDebugDraw() const
    {
        return getDebugDraw();
    }

    //--------------------------------------------------------------------------------------
    DebugDraw * Renderer::getDebugDraw() const
    {
        return DebugDraw::get();
    }

    //--------------------------------------------------------------------------------------
    IUIManager * Renderer::GetUIManager() const
    {
        return UIManager::get();
    }

    //--------------------------------------------------------------------------------------
    ILightInstance * Renderer::CreateLightInstance(const ILightDesc * _lightDesc)
    {
        VG_ASSERT(_lightDesc);

        if (_lightDesc)
        {
            const LightType lightType = _lightDesc->GetLightType();
            switch (lightType)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(lightType);
                    return nullptr;

                case LightType::Directional:
                    return new DirectionalLightInstance((const DirectionalLightDesc *)_lightDesc);

                case LightType::Omni:
                    return new OmniLightInstance((const OmniLightDesc *)_lightDesc);
            }
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool Renderer::IsHDRSupported(gfx::HDR _mode) const
    {
        return m_device.getDeviceCaps().hdr.isSupported(_mode);
    }

    //--------------------------------------------------------------------------------------
    bool Renderer::IsMSAASupported(gfx::MSAA _mode) const
    {
        return m_device.getDeviceCaps().msaa.isSupported(_mode);
    }

    //--------------------------------------------------------------------------------------
    bool Renderer::IsVSyncSupported(gfx::VSync _mode) const
    {
        return m_device.isVSyncSupported(_mode);
    }

    //--------------------------------------------------------------------------------------
    double Renderer::GetGpuFrameTime() const
    {
        return m_device.getGpuFrameTime();
    }

    //--------------------------------------------------------------------------------------
    double Renderer::GetGpuWaitTime() const
    {
        return m_device.getGpuWaitTime();
    }

    //--------------------------------------------------------------------------------------
    const gfx::DeviceCaps & Renderer::getDeviceCaps() const
    {
        return m_device.getDeviceCaps();
    }

    //--------------------------------------------------------------------------------------
    gfx::Texture * Renderer::getSpecularBRDF() const
    {
        if (asBool(PBRFlags::GenerateSpecularBRDF & RendererOptions::get()->getPBRFlags()))
            return m_generatedSpecularBRDF;
        else
            return m_bakedSpecularBRDF;
    }

    //--------------------------------------------------------------------------------------
    void Renderer::releaseSpecularBRDF()
    {
        VG_SAFE_RELEASE(m_generatedSpecularBRDF);
        if (m_computeSpecularBRDFPass)
            m_computeSpecularBRDFPass->setSpecularBRDFTexture(nullptr);
    }
}