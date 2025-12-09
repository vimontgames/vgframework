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
#include "gfx/Importer/MeshImporterSettings.h"

#include "renderer/RenderPass/ImGui/ImGui.h"
#include "renderer/RenderPass/ImGui/ImGuiPass.h"
#include "renderer/RenderPass/ImGui/imguiAdapter.h"
#include "renderer/RenderPass/Update/GPUDebug/GPUDebugUpdatePass.h"
#include "renderer/RenderPass/Update/InstanceData/InstanceDataUpdatePass.h"
#include "renderer/RenderPass/Update/MaterialData/MaterialDataUpdatePass.h"
#include "renderer/RenderPass/Compute/ComputeSkinning/ComputeSkinningPass.h"
#include "renderer/RenderPass/Update/ParticleData/ParticleRenderDataUpdatePass.h"
#include "renderer/RenderPass/Update/BLAS/BLASUpdatePass.h"
#include "renderer/RenderPass/Update/TLAS/TLASUpdatePass.h"
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
#include "renderer/Model/Material/MaterialManager.h"
#include "renderer/Particle/ParticleManager.h"
#include "renderer/Camera/CameraLens.h"
#include "renderer/Job/Culling/WorldCullingJob.h"

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

#ifdef _WIN32
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

//--------------------------------------------------------------------------------------
namespace vg::renderer
{
    #ifdef _WIN32
    //--------------------------------------------------------------------------------------
    LRESULT CALLBACK Renderer::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
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

        // Profiler singleton
        _singletons.profiler = Kernel::getProfiler();

        // Get other Singletons for Renderer
        Kernel::setSingletons(_singletons);

        // Does this renderer support editor rendering?
        m_editor = _params.editor;

        // Start profiling immediately if requested
        if (_params.profileStart)
            GetProfiler()->Start();

        VG_PROFILE_CPU_EVENT_START("Init");

        // We can profile from now
        {
            VG_PROFILE_CPU("Renderer");

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
            m_materialDataUpdatePass = new MaterialDataUpdatePass();
            m_computeSkinningPass = new ComputeSkinningPass();
            m_particleDataUpdatePass = new ParticleRenderDataUpdatePass();
            m_computeSpecularBRDFPass = new ComputeSpecularBRDFPass();
            m_computeIBLCubemapsPass = new ComputeIBLCubemapsPass();
            m_BLASUpdatePass = new BLASUpdatePass();
            m_TLASUpdatePass = new TLASUpdatePass();
            m_imguiPass = new ImGuiPass();
            m_hdrOutputPass = new HDROutputPass();

            RegisterClasses();

            RayTracingManager * rtManager = new RayTracingManager();

            m_rendererOptionsMainThread = new RendererOptions("Renderer Options", this);
            m_rendererOptionsSafeCopy = new RendererOptions("Renderer Options (Safe copy)", this, true);
            m_rendererOptionsSafeCopy->sync(*m_rendererOptionsMainThread);

            m_materialManager = new MaterialManager();
            m_particleManager = new ParticleManager();

            initDefaultTextures();
            initDefaultMaterials();

            // Shared job output (must be created before views because it's needed to init View culling jobs
            m_sharedWorldCullingJobOutput = new SharedWorldCullingJobOutput();
            m_sharedViewCullingJobOutput = new SharedViewCullingJobOutput();

            // Create "Game" viewport for "Game" views
            auto gameViewportParams = CreateViewportParams(ViewportTarget::Game, GetBackbufferSize());
            if (auto gameViewport = (Viewport *)CreateViewport(gameViewportParams, "Game"))
            {
                AddViewport(gameViewport);
                VG_SAFE_RELEASE(gameViewport);
            }

            m_defaultCameraLens = new CameraLens("Default 24-70mm FullFrame");
        }
	}

    //--------------------------------------------------------------------------------------
    const ICameraLens * Renderer::GetDefaultCameraLens() const
    {
        return m_defaultCameraLens;
    }

    //--------------------------------------------------------------------------------------
    void Renderer::registerShaders()
    {
        VG_PROFILE_CPU("Shaders");

        auto * shaderManager = ShaderManager::get();

        // TODO: register from parsing 
        shaderManager->registerHLSL(DriverHLSLDesc());
        shaderManager->registerHLSL(DebugDrawHLSLDesc());
        shaderManager->registerHLSL(DefaultHLSLDesc());
        shaderManager->registerHLSL(BackgroundHLSLDesc());
        shaderManager->registerHLSL(PostProcessHLSLDesc());
        shaderManager->registerHLSL(SkinningHLSLDesc());
        shaderManager->registerHLSL(DeferredLightingHLSLDesc());
        shaderManager->registerHLSL(PreviewHLSLDesc());
        shaderManager->registerHLSL(PrecomputeIBLHLSLDesc());

        // Register callback for after shader updates
        shaderManager->setOnShadersUpdatedCallback(onShadersUpdated);

        // First update will build CRC
        shaderManager->update(true);

        // Load from cache after CRC has been computed
        shaderManager->loadFromCache();
    }

	//--------------------------------------------------------------------------------------
	void Renderer::Deinit()
	{
        VG_SAFE_RELEASE(m_defaultCameraLens);
        VG_SAFE_RELEASE(m_generatedSpecularBRDF);
        VG_SAFE_DELETE(m_sharedViewCullingJobOutput);
        VG_SAFE_DELETE(m_sharedWorldCullingJobOutput);
        VG_SAFE_RELEASE(m_hdrOutput);

        UnregisterClasses();

        deinitDefaultMaterials();
        deinitDefaultTextures();

        VG_SAFE_DELETE(m_rendererOptionsSafeCopy);
        VG_SAFE_DELETE(m_rendererOptionsMainThread);

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
        VG_SAFE_RELEASE(m_materialDataUpdatePass);
        VG_SAFE_DELETE(m_computeSkinningPass);
        VG_SAFE_DELETE(m_particleDataUpdatePass);
        VG_SAFE_DELETE(m_computeSpecularBRDFPass);
        VG_SAFE_DELETE(m_computeIBLCubemapsPass);
        VG_SAFE_DELETE(m_BLASUpdatePass);
        VG_SAFE_DELETE(m_TLASUpdatePass);
        VG_SAFE_DELETE(m_imguiPass);
        VG_SAFE_DELETE(m_imgui);
        VG_SAFE_DELETE(m_hdrOutputPass);

        m_frameGraph.Release();
        
		m_device.deinit();

        VG_SAFE_DELETE(m_particleManager);
        VG_SAFE_DELETE(m_materialManager);
        VG_SAFE_RELEASE(m_picking);
	}

    //--------------------------------------------------------------------------------------
    void Renderer::CreateResources()
    {
        ((RendererOptions*)GetOptions())->createResources();
    }

    //--------------------------------------------------------------------------------------
    void Renderer::ReleaseResources()
    {
        ((RendererOptions *)GetOptions())->releaseResources();
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
    // TODO: detect the source envmap used to compute irradiance/specular only once 
    //--------------------------------------------------------------------------------------
    void Renderer::updateIBLCubemaps()
    {
        RendererOptions * options = RendererOptions::get();

        const PBRFlags pbrFlags = options->getPBRFlags();
        
        bool anyRecompute = false;

        const auto & visibleWorlds = getSharedWorldCullingJobOutput()->m_allVisibleWorlds;

        for (uint i = 0; i < visibleWorlds.size(); ++i)
        {
            auto * world = visibleWorlds[i].m_world;

            bool recomputeIrradiance = false, recomputeSpecularReflection = false;

            Texture * environmentCubemap = (Texture *)world->GetEnvironmentCubemap();
            Texture * irradianceCubemap = nullptr;

            if (nullptr != environmentCubemap && asBool(PBRFlags::GenerateIrradianceCubemap & pbrFlags))
            {
                irradianceCubemap = (Texture *)world->GetIrradianceCubemap();

                TextureDesc irradianceCubemapDesc = environmentCubemap->getTexDesc();
                irradianceCubemapDesc.resource.m_bindFlags = BindFlags::ShaderResource | BindFlags::UnorderedAccess;
                irradianceCubemapDesc.format = PixelFormat::R16G16B16A16_float;
                irradianceCubemapDesc.width = 16;
                irradianceCubemapDesc.height = 16;
                irradianceCubemapDesc.mipmaps = 1;

                if (nullptr != irradianceCubemap && irradianceCubemapDesc != irradianceCubemap->getTexDesc())
                {
                    world->SetIrradianceCubemap(nullptr);
                    irradianceCubemap = nullptr;
                }

                if (nullptr == irradianceCubemap)
                {
                    irradianceCubemap = m_device.createTexture(irradianceCubemapDesc, "Irradiance cubemap");
                    world->SetIrradianceCubemap(irradianceCubemap);
                    irradianceCubemap->Release();
                    recomputeIrradiance = true;
                }
            }
            else
            {
                world->SetIrradianceCubemap(nullptr);
            }

            Texture * specularReflectionCubemap = nullptr;
            if (nullptr != environmentCubemap && asBool(PBRFlags::GenerateSpecularReflectionCubemap & pbrFlags))
            {
                specularReflectionCubemap = (Texture *)world->GetSpecularReflectionCubemap();

                TextureDesc specularReflectionCubemapDesc = environmentCubemap->getTexDesc();
                specularReflectionCubemapDesc.resource.m_bindFlags = BindFlags::ShaderResource | BindFlags::UnorderedAccess;
                specularReflectionCubemapDesc.format = PixelFormat::R16G16B16A16_float;

                if (nullptr != specularReflectionCubemap && specularReflectionCubemapDesc != specularReflectionCubemap->getTexDesc())
                {
                    world->SetSpecularReflectionCubemap(nullptr);
                    specularReflectionCubemap = nullptr;
                }

                if (nullptr == specularReflectionCubemap)
                {
                    specularReflectionCubemap = m_device.createTexture(specularReflectionCubemapDesc, "Specular Reflection cubemap");
                    world->SetSpecularReflectionCubemap(specularReflectionCubemap);
                    specularReflectionCubemap->Release();
                    recomputeSpecularReflection = true;
                }
            }
            else
            {
                world->SetSpecularReflectionCubemap(nullptr);
            }

            if (recomputeIrradiance || recomputeSpecularReflection)
            {
                if (asBool((PBRFlags::GenerateIrradianceCubemap | PBRFlags::GenerateSpecularReflectionCubemap) & pbrFlags))
                {
                    m_computeIBLCubemapsPass->add(environmentCubemap, recomputeIrradiance ? irradianceCubemap : nullptr, recomputeSpecularReflection ? specularReflectionCubemap : nullptr);
                    anyRecompute = true;
                }
            }
        }

        if (anyRecompute)
        {
            RenderPassContext renderPassContext;
            renderPassContext.setWorld(nullptr);
            renderPassContext.setView(nullptr);
            m_frameGraph.addUserPass(renderPassContext, m_computeIBLCubemapsPass, "Compute IBL Cubemaps");
        }
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

        RendererOptions * options = m_rendererOptionsMainThread;
        if (!options)
            return;

        // Copy engine values for render
        m_rendererOptionsSafeCopy->sync(*m_rendererOptionsMainThread);
        
        options->Update();

        if (options->isRenderJobsEnabled())
            m_device.setupRenderJobs(options->getRenderJobCount(), options->getRenderJobsPolicy(), options->getMaxRenderTotalBufferSize(), options->getMaxRenderMinBufferSize());
        else
            m_device.setupRenderJobs(0, options->getRenderJobsPolicy(), options->getMaxRenderTotalBufferSize(), options->getMaxRenderMinBufferSize());

		m_device.beginFrame();
		{
            //--------------------------------------------------------------------------------------
            // List all worlds and their visible views, and instances (but no frustum culling)
            //--------------------------------------------------------------------------------------
            cullWorlds();

            // Update world IBL cubemaps if needed (must happen after cullWorlds)
            updateIBLCubemaps();

            // Update all particles (visible or not) before culling
            m_particleManager->updateSimulation();

            //--------------------------------------------------------------------------------------
            // Cull all views and wait (TODO: split in several jobs per view)
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

                // RenderPass context for global updates not linked to a world or view
                RenderPassContext mainViewRenderPassContext;
                mainViewRenderPassContext.setWorld(nullptr);
                mainViewRenderPassContext.setView(nullptr);

                m_frameGraph.addUserPass(mainViewRenderPassContext, m_gpuDebugUpdatePass, "GPU Debug");

                // Skinning shall be computed before instances so that instances can store the vertex data offset in GPUInstanceData
                m_frameGraph.addUserPass(mainViewRenderPassContext, m_computeSkinningPass, "Skinning");

                if (options->isParticlesEnabled())
                    m_frameGraph.addUserPass(mainViewRenderPassContext, m_particleDataUpdatePass, "ParticleData Update");

                m_frameGraph.addUserPass(mainViewRenderPassContext, m_instanceDataUpdatePass, "Instance Data");
                m_frameGraph.addUserPass(mainViewRenderPassContext, m_materialDataUpdatePass, "Material Data");

                if (RendererOptions::get()->isRayTracingEnabled())
                {
                    m_frameGraph.addUserPass(mainViewRenderPassContext, m_BLASUpdatePass, "BLAS");
                
                    if (options->getRayTracingTLASMode() == TLASMode::PerWorld)
                    {
                        const core::vector<WorldCullingJobOutput> & visibleWorlds = getSharedWorldCullingJobOutput()->m_allVisibleWorlds;
                        for (uint w = 0; w < visibleWorlds.size(); ++w)
                        {
                            const WorldCullingJobOutput & visibleWorld = visibleWorlds[w];

                            RenderPassContext worldTLASRenderPassContext;
                            worldTLASRenderPassContext.setWorld(visibleWorld.m_world);
                            worldTLASRenderPassContext.setViews((core::vector<IFrameGraphView *>&)visibleWorld.m_views);

                            m_frameGraph.addUserPass(worldTLASRenderPassContext, m_TLASUpdatePass, "World TLAS");
                        }
                    }
                }

                if (asBool(PBRFlags::GenerateSpecularBRDF & options->getPBRFlags()))
                {
                    // Generate specular BRDF LUT if dirty
                    if (nullptr == m_generatedSpecularBRDF)
                    {
                        TextureDesc specularBRDFDesc = TextureDesc(Usage::Default, BindFlags::ShaderResource | BindFlags::UnorderedAccess, CPUAccessFlags::None, TextureType::Texture2D, PixelFormat::R16G16_float, TextureFlags::None, 256, 256, 1, 1, MSAA::None);
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

                const auto & visibleWorlds = getSharedWorldCullingJobOutput()->m_allVisibleWorlds;

                for (uint w = 0; w < visibleWorlds.size(); ++w)
                {
                    const auto & visibleViews = visibleWorlds[w].m_views;
                    for (uint v = 0; v < visibleViews.size(); ++v)
                    {
                        View * view = (View *)visibleViews[v];

                        gfx::RenderPassContext rc;
                        rc.setView(view);
                        rc.setWorld(view->getWorld());

                        view->RegisterFrameGraph(rc, m_frameGraph);
                    }
                }

                // Additional preview passes
                m_imgui->RegisterFrameGraph(mainViewRenderPassContext, m_frameGraph);

                m_frameGraph.addUserPass(mainViewRenderPassContext, m_imguiPass, "ImGui");

                if (hdrOutput)
                    m_frameGraph.addUserPass(mainViewRenderPassContext, m_hdrOutputPass, "HDROutput");

                m_frameGraph.resize();
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

    static volatile int counter = 0;

    //--------------------------------------------------------------------------------------
    void Renderer::cullWorlds()
    {
        VG_ASSERT(m_sharedWorldCullingJobOutput);
        m_sharedWorldCullingJobOutput->clear();

        auto & visibleWorlds = m_sharedWorldCullingJobOutput->m_allVisibleWorlds;
      
        counter = 0;
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

                    auto * world = view->GetWorld();

                    if (nullptr != world)
                    {
                        auto it = std::find_if(visibleWorlds.begin(), visibleWorlds.end(), [&](const WorldCullingJobOutput & _other) { return _other.m_world == world; });
                        WorldCullingJobOutput * visibleWorld;
                        if (visibleWorlds.end() != it)
                            visibleWorld = &(*it);
                        else
                            visibleWorld = &visibleWorlds.emplace_back(world);

                        visibleWorld->m_views.push_back(view);
                        counter++;
                    }
                }
            }
        }

        core::Scheduler * jobScheduler = (core::Scheduler *)Kernel::getScheduler();
        auto jobSync = GetJobSync(RendererJobType::WorldCulling);

        uint jobStartCounter = 0;
        for (uint w = 0; w < visibleWorlds.size(); ++w)
        {
            WorldCullingJobOutput & visibleWorld = visibleWorlds[w];

            auto * job = visibleWorld.getCullingJob();
            jobScheduler->Start(job, jobSync);
            jobStartCounter++;
        }

        if (jobStartCounter > 0)
        {
            VG_PROFILE_CPU("Wait World Culling");
            WaitJobSync(RendererJobType::WorldCulling);
        }
    }

    //--------------------------------------------------------------------------------------
    void Renderer::cullViews()
    {
        VG_ASSERT(m_sharedViewCullingJobOutput);
        m_sharedViewCullingJobOutput->clear();

        core::Scheduler * jobScheduler = (core::Scheduler *)Kernel::getScheduler();
        auto jobSync = GetJobSync(RendererJobType::ViewCulling);

        // Perform culling for each view (might want to split views later)
        uint jobStartCounter = 0;

        #if 1
        const SharedWorldCullingJobOutput * worldCulling = getSharedWorldCullingJobOutput();

        for (uint w = 0; w < worldCulling->m_allVisibleWorlds.size(); ++w)
        {
            const auto & views = worldCulling->m_allVisibleWorlds[w].m_views;
            for (uint v = 0; v < views.size(); ++v)
            {
                View * view = (View*)views[v];
                auto * job = view->getCullingJob();
                jobScheduler->Start(job, jobSync);
                jobStartCounter++;
            }
        }
        #else
  
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
        #endif

        if (jobStartCounter > 0)
        {
            VG_PROFILE_CPU("Wait View Culling");
            WaitJobSync(RendererJobType::ViewCulling);
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
    const renderer::IRendererOptions * Renderer::GetOptions() const
    {
        if (auto * scheduler = Scheduler::get(false))
            VG_ASSERT(scheduler->IsMainThread());
        return m_rendererOptionsMainThread;
    }

    //--------------------------------------------------------------------------------------
    renderer::IRendererOptions * Renderer::GetOptions()
    {
        if (auto * scheduler = Scheduler::get(false))
            VG_ASSERT(scheduler->IsMainThread());
        return m_rendererOptionsMainThread;
    }

    //--------------------------------------------------------------------------------------
    gfx::ITexture * Renderer::CreateTexture(const gfx::TextureDesc & _texDesc, const core::string & _name)
    {
        auto tex = m_device.createTexture(_texDesc, _name);
        tex->SetName(_name);
        return tex;
    }

    //--------------------------------------------------------------------------------------
    IViewport * Renderer::CreateViewport(const CreateViewportParams & _params, const core::string & _name, ViewportFlags _flags)
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
    gfx::ViewportID Renderer::AddViewport(IViewport * _viewport)
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
    IViewport * Renderer::GetViewport(gfx::ViewportID _viewportID)
    {
        const auto & viewports = m_viewports[asInteger(_viewportID.target)];
        if (_viewportID.index < viewports.size())
            return viewports[_viewportID.index];
        else
            return nullptr;
    }

    //--------------------------------------------------------------------------------------
    const core::vector<IViewport *> & Renderer::GetViewports(gfx::ViewportTarget _target) const
    {
        return (core::vector<IViewport *> &)m_viewports[asInteger(_target)];
    }

    //--------------------------------------------------------------------------------------
    IView * Renderer::CreateView(CreateViewParams _params, const core::string & _name, ViewFlags _flags)
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
    gfx::ViewID Renderer::AddView(IView * _view)
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
    IView * Renderer::GetView(gfx::ViewID _viewID) const
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
    IView * Renderer::GetView(gfx::ViewTarget _target, const core::IWorld * _world) const
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
                    return (IView *)view;
            }
        }
        return nullptr;
    }
    
    //--------------------------------------------------------------------------------------
    bool Renderer::CookMeshModel(const core::string & _file, const gfx::MeshImporterSettings * _importSettings)
    {
        SceneImporterData imported;

        if (m_fbxImporter->ImportFBX(_file, imported, _importSettings))
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
    LoadStatus Renderer::LoadMeshModel(const core::string & _file, IMeshModel *& _meshModel)
    {
        MeshImporterData meshData;

        LoadStatus status = meshData.load(io::getCookedPath(_file));

        if (status == LoadStatus::Success)
            return MeshModel::createFromImporterData(meshData, _meshModel);

        _meshModel = nullptr;
        return status;
    }

    //--------------------------------------------------------------------------------------
    bool Renderer::CookTexture(const core::string & _file, const gfx::TextureImporterSettings * _importSettings)
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
    LoadStatus Renderer::LoadTexture(const core::string & _file, gfx::ITexture *& _texture)
    {
        TextureImporterData textureData;

        LoadStatus status = textureData.load(io::getCookedPath(_file));
        if (status == LoadStatus::Success)
            return m_device.createResourceTexture(textureData.desc, textureData.name, textureData.texels.data(), (gfx::Texture * &)_texture);

        _texture = nullptr;
        return status;
    }

    //--------------------------------------------------------------------------------------
    bool Renderer::CookAnimation(const core::string & _file)
    {
        SceneImporterData imported;

        if (m_fbxImporter->ImportFBX(_file, imported, nullptr))
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
    LoadStatus Renderer::LoadAnimation(const core::string & _file, IAnimation *& _animation)
    {
        AnimImporterData animData;

        LoadStatus status = animData.load(io::getCookedPath(_file));

        if (status == LoadStatus::Success)
        {
            if (IAnimation * anim = SkeletalAnimation::createFromImporterData(animData))
            {
                _animation = anim;
                return LoadStatus::Success;
            }
            else
            {
                _animation = nullptr;
                return LoadStatus::CannotCreateObject;
            }
        }

        _animation = nullptr;
        return LoadStatus::CannotOpenFile;
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
                    data = 0x00FF7F7F;
                    reservedSlot = ReservedSlot::DefaultNormalTexSrv;
                    break;

                case MaterialTextureType::PBR:
                    data = 0x007F7FFF;    // Blue: Metalness Green: Roughness Red: Occlusion 
                    reservedSlot = ReservedSlot::DefaultPBRTexSrv;
                    break;

                case MaterialTextureType::Emissive:
                    data = 0x00000000;    
                    reservedSlot = ReservedSlot::DefaultEmissiveTexSrv;
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
        VG_ASSERT(m_defaultMaterial->getGPUMaterialDataIndex() == 0);
    }

    //--------------------------------------------------------------------------------------
    void Renderer::deinitDefaultMaterials()
    {
        VG_SAFE_RELEASE(m_defaultMaterial);
    }

    //--------------------------------------------------------------------------------------
    void Renderer::SetFullscreen(bool _fullscreen)
    {
        // Always maximize game view in standalone
        if (!m_editor)
            _fullscreen = true;

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
        if (m_editor)
            return m_fullscreen;
        else
            return true;    // Game view is always maximized in standalone            
    }

    //--------------------------------------------------------------------------------------
    bool Renderer::IsEditor() const
    {
        return m_editor;
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
    bool Renderer::GetGpuMemoryInfo(core::GPUMemoryInfo & _gpuMem) const
    {
        return m_device.GetGpuMemoryInfo(_gpuMem);
    }

    //--------------------------------------------------------------------------------------
    core::u64 Renderer::GetAvailableUploadSize() const
    {
        return m_device.getAvailableUploadSize();
    }

    //--------------------------------------------------------------------------------------
    core::u64 Renderer::GetTotalUploadSize() const
    {
        return m_device.getTotalUploadSize();
    }

    //--------------------------------------------------------------------------------------
    bool Renderer::IsReadyForStreaming() const
    {
        return m_device.isReadyForStreaming();
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
            return (Texture*)GetOptions()->GetBakedSpecularBRDF();
    }

    //--------------------------------------------------------------------------------------
    void Renderer::releaseSpecularBRDF()
    {
        VG_SAFE_RELEASE(m_generatedSpecularBRDF);
        if (m_computeSpecularBRDFPass)
            m_computeSpecularBRDFPass->setSpecularBRDFTexture(nullptr);
    }

    //--------------------------------------------------------------------------------------
    core::JobSync * Renderer::GetJobSync(RendererJobType _jobSync) 
    {
        auto & info = m_jobSyncInfo[core::asInteger(_jobSync)];
        info.started.store(true);
        return &info.id;
    }

    //--------------------------------------------------------------------------------------
    bool Renderer::WaitJobSync(RendererJobType _jobSync)
    {
        auto * scheduler = Kernel::getScheduler();
        auto & info = m_jobSyncInfo[core::asInteger(_jobSync)];

        if (info.started)
        {
            scheduler->Wait(&info.id);
            info.started = false;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    core::u64 Renderer::getFrameCounter() const
    {
        return m_device.getFrameCounter();
    }
}