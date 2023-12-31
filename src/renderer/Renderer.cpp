#pragma once

#include "renderer/Precomp.h"
#include "renderer.h"

#include "core/Kernel.h"
#include "core/Timer/Timer.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/File/File.h"
#include "core/Scheduler/Scheduler.h"

#include "gfx/device/device.h"
#include "gfx/Shader/ShaderManager.h"
#include "gfx/FrameGraph/FrameGraph.h"
#include "gfx/Profiler/Profiler.h"
#include "gfx/Resource/Texture.h"
#include "gfx/Importer/TextureImporter.h"

#include "renderer/RenderPass/ImGui/ImGui.h"
#include "renderer/RenderPass/ImGui/ImGuiPass.h"
#include "renderer/RenderPass/ImGui/imguiAdapter.h"
#include "renderer/RenderPass/Compute/ComputeSkinning/ComputeSkinningPass.h"
#include "renderer/RenderPass/Update/BLAS/BLASUpdatePass.h"
#include "renderer/Importer/SceneImporterData.h"
#include "renderer/Model/Mesh/MeshModel.h"
#include "renderer/Animation/SkeletalAnimation.h"
#include "renderer/Options/RendererOptions.h"
#include "renderer/IGraphicInstance.h"
#include "renderer/Importer/TextureImporterData.h"
#include "renderer/View/View.h"
#include "renderer/View/Lit/LitView.h"
#include "renderer/Model/Material/DefaultMaterial/DefaultMaterialModel.h"
#include "renderer/Picking/PickingManager.h"
#include "renderer/DebugDraw/DebugDraw.h"
#include "renderer/RayTracing/RayTracingManager.h"
#include "renderer/Instance/Light/Directional/DirectionalLightInstance.h"
#include "renderer/Instance/Light/Omni/OmniLightInstance.h"

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
	#define VG_RENDERER_VERSION_MAJOR 0
	#define VG_RENDERER_VERSION_MINOR 1

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
	core::IPlugin::Version Renderer::GetVersion() const
	{
		return { VG_RENDERER_VERSION_MAJOR, VG_RENDERER_VERSION_MINOR };
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
		m_device.release();
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
        IFactory * factory = Kernel::getFactory();
        return AutoRegisterClassInfo::unregisterClasses(*factory);
    }

    //--------------------------------------------------------------------------------------
    bool Renderer::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

	//--------------------------------------------------------------------------------------
	void Renderer::init(const RendererCreationParams & _params, core::Singletons & _singletons)
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

        registerShaders();

        // Debug draw (singleton)
        DebugDraw * dbgDraw = new DebugDraw();

        // Create passes not bound to a View
        m_computeSkinningPass = new ComputeSkinningPass();
        m_BLASUpdatePass = new BLASUpdatePass();
        m_imguiPass = new ImGuiPass();

        RegisterClasses();

        RendererOptions * options = new RendererOptions("Renderer Options", this);

        initDefaultTextures();
        initDefaultMaterials();

        RayTracingManager * rtManager = new RayTracingManager();

        // Shared job output (must be created before views because it's needed to init View culling jobs
        m_sharedCullingJobOutput = new SharedCullingJobOutput();

        // Create main view
        auto mainViewParams = gfx::CreateViewParams(gfx::ViewTarget::Backbuffer, getBackbufferSize());
        m_mainView = (View*)CreateView(mainViewParams, "MainView");

        auto gameView0Params = gfx::CreateViewParams(gfx::ViewTarget::Game, getBackbufferSize());
        auto gameView = (View *)CreateView(gameView0Params, "GameView#0");
        AddView(gameView);
        VG_SAFE_RELEASE(gameView);
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

        sm->update();
    }

	//--------------------------------------------------------------------------------------
	void Renderer::deinit()
	{
        VG_SAFE_DELETE(m_sharedCullingJobOutput);

        UnregisterClasses();

        deinitDefaultMaterials();
        deinitDefaultTextures();

        RendererOptions * options = RendererOptions::get();
        VG_SAFE_DELETE(options);

        DebugDraw * dbgDraw = getDebugDraw();
        VG_SAFE_RELEASE(dbgDraw);

        RayTracingManager * rtManager = RayTracingManager::get();
        VG_SAFE_RELEASE(rtManager);

        VG_SAFE_RELEASE(m_mainView);
        for (uint j = 0; j < core::enumCount<gfx::ViewTarget>(); ++j)
        {
            auto & views = m_views[j];
            for (uint i = 0; i < views.size(); ++i)
                VG_SAFE_RELEASE(views[i]);
            views.clear();
        }

        VG_SAFE_DELETE(m_computeSkinningPass);
        VG_SAFE_DELETE(m_BLASUpdatePass);
        VG_SAFE_DELETE(m_imguiPass);
        VG_SAFE_DELETE(m_imgui);

        m_frameGraph.release();

		m_device.deinit();

        VG_SAFE_RELEASE(m_picking);
	}

    //--------------------------------------------------------------------------------------
    void Renderer::SetResized()
    {
        m_frameGraph.setResized();
    }

    //--------------------------------------------------------------------------------------
    void Renderer::resize(core::uint _width, core::uint _height)
    {
        m_device.waitGPUIdle();
        m_device.resize(_width, _height);
        m_mainView->setSize(uint2(_width, _height));

        if (IsFullscreen())
        {
            auto & views = m_views[(int)ViewTarget::Game];
            for (uint i = 0; i < views.count(); ++i)
            {
                auto * view = views[i];
                if (nullptr != view)
                {
                    view->setSize(m_mainView->getSize());
                    view->SetActive(true);
                    view->SetVisible(true);
                }
            }
        }

        m_frameGraph.destroyTransientResources();
        DebugDraw::get()->reset();
    }

    //--------------------------------------------------------------------------------------
    core::uint2 Renderer::getBackbufferSize() const
    {
        const auto & desc = getBackbuffer()->getTexDesc();
        return { desc.width, desc.height };
    }

    //--------------------------------------------------------------------------------------
    void Renderer::updateShaders()
    {
        m_device.getShaderManager()->update();
    }

    //--------------------------------------------------------------------------------------
    void Renderer::waitGPUIdle()
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
	void Renderer::runOneFrame(float _dt)
	{
        if (m_device.isMinimized())
        {
            Sleep(1);
            DebugDraw::get()->endFrame();
            return;
        }

        VG_PROFILE_CPU("Renderer");

        const RendererOptions * options = RendererOptions::get();

		m_device.beginFrame();
		{
            if (!m_fullscreen)
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

                m_frameGraph.importRenderTarget("Backbuffer", m_device.getBackbuffer(), float4(0,0,0,0), FrameGraphResource::InitState::Clear);
                m_frameGraph.setGraphOutput("Backbuffer");

                // Register passes not linked to views (e.g. skinning, or BLAS updates)
                RenderPassContext mainViewRenderPassContext;
                                  mainViewRenderPassContext.m_view = m_mainView;

                m_frameGraph.addUserPass(mainViewRenderPassContext, m_computeSkinningPass, "Skinning");

                if (options->isRayTracingEnabled())
                    m_frameGraph.addUserPass(mainViewRenderPassContext, m_BLASUpdatePass, "BLAS Update");

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
                    for (uint i = 0; i < views.count(); ++i)
                    {
                        auto * view = (View*)views[i];
                        if (nullptr != view)
                        {
                            if (!view->IsVisible())
                                continue;

                            gfx::RenderPassContext rc;
                                                   rc.m_view = view;

                            view->RegisterFrameGraph(rc, m_frameGraph);
                        }
                    }
                }

                if (!m_fullscreen)
                    m_frameGraph.addUserPass(mainViewRenderPassContext, m_imguiPass, "ImGui");

                m_frameGraph.setup(_dt);
                m_frameGraph.build();
                m_frameGraph.render();

                // reset debug draw after last render
                DebugDraw::get()->endFrame();
            }
		}
		m_device.endFrame();
	}

    //--------------------------------------------------------------------------------------
    // TODO: create additional views from Views (Shadowmaps, Cubemaps ...)
    //--------------------------------------------------------------------------------------
    void Renderer::cullViews()
    {
        m_sharedCullingJobOutput->clear();

        core::Scheduler * jobScheduler = (core::Scheduler *)Kernel::getScheduler();

        // Perform culling for each view (might want to split views later)
        uint jobStartCounter = 0;
        core::JobSync syncCull;
        for (uint j = 0; j < core::enumCount<gfx::ViewTarget>(); ++j)
        {
            const auto & views = m_views[j];
            for (uint i = 0; i < views.size(); ++i)
            {
                View * view = views[i];
                if (view && view->IsVisible())
                {
                    auto * job = view->getCullingJob();
                    jobScheduler->Start(job, &syncCull);
                    jobStartCounter++;
                }
            }
        }

        if (jobStartCounter > 0)
        {
            VG_PROFILE_CPU("syncCull");
            jobScheduler->Wait(syncCull);
        }
    }

    //--------------------------------------------------------------------------------------
    View * Renderer::getMainView() const
    {
        return m_mainView;
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
        tex->setName(_name);
        return tex;
    }

    //--------------------------------------------------------------------------------------
    gfx::IView * Renderer::CreateView(gfx::CreateViewParams _params, const core::string & _name, IView::Flags _flags)
    {
        View * view;

        switch (_params.target)
        {
            default:
                view = new LitView(_params);
                break;

            case ViewTarget::Backbuffer:
                view = new View(_params);
                VG_ASSERT(m_mainView == nullptr, "Only one \"Main\" backbuffer view is supported");
                m_mainView = static_cast<View *>(view); // This is required for FrameGraph construction
                break;
        }
        
        view->setName(_name);
        view->setFlags(_flags);
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
                views[i] = (View*)_view;
                auto id = ViewID(target, i);
                _view->SetViewID(id);
                return id;
            }
        }

        auto index = (ViewIndex)views.count();
        ViewID id = ViewID(target, index);
        views.push_back((View *)_view);
        _view->SetViewID(id);
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
    gfx::IView * Renderer::GetView(gfx::ViewID _viewID) const
    {
        const auto & views = m_views[(uint)_viewID.target];
        if (_viewID.index < views.size())
            return views[_viewID.index];
        else
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
    bool Renderer::cookTexture(const core::string & _file)
    {
        TextureImporterData imported;
        imported.name = _file;

        if (TextureImporter::get()->importTextureData(_file, imported.desc, imported.texels))
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
            switch (type)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                    break;

                case MaterialTextureType::Albedo:
                    data = 0xFFBBBBBB;
                    break;

                case MaterialTextureType::Normal:
                    data = 0xFFFF7F7F;
                    break;

                case MaterialTextureType::PBR:
                    data = 0x00007FFF;    // Blue: Metalness Green: Roughness Red: Occlusion
                    break;
            }

            TextureDesc texDesc = TextureDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, TextureType::Texture2D, PixelFormat::R8G8B8A8_unorm, TextureFlags::None, 1, 1);
            string name = "Default_" + asString(type);
            m_defaultTextures[t] = m_device.createTexture(texDesc, name.c_str(), &data);
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
        if (_fullscreen != m_fullscreen)
        {
            m_fullscreen = _fullscreen;
            VG_INFO("[Renderer] Set Fullscreen to '%s'", _fullscreen ? "true" : "false");
            //m_device.waitGPUIdle();
            //m_frameGraph.destroyTransientResources();
            //DebugDraw::get()->reset();

            if (_fullscreen)
            {
                // Resize view to fit backbuffer
                auto & views = m_views[(int)ViewTarget::Game];
                for (uint i = 0; i < views.count(); ++i)
                {
                    auto * view = views[i];
                    if (nullptr != view)
                    {
                        view->setSize(m_mainView->getSize());
                        view->SetActive(true);
                        view->SetVisible(true);
                    }
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    bool Renderer::IsFullscreen() const
    {
        return m_fullscreen;
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
}